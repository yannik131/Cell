#include "core/Simulation.hpp"
#include "SFMLJsonSerializers.hpp"
#include "cell/Cell.hpp"
#include "cell/ExceptionWithLocation.hpp"
#include "core/SimulationConfigUpdater.hpp"

#include <QThread>
#include <QTimer>
#include <SFML/System/Clock.hpp>
#include <nlohmann/json.hpp>

#include <fstream>

using json = nlohmann::json;

Simulation::Simulation(QObject* parent)
    : QObject(parent)
{
    QTimer::singleShot(0, this, &Simulation::loadDefaultConfig);
}

void Simulation::run()
{
    sf::Clock clock;
    double timeSinceLastUpdate;

    // Since every change to the config causes an immediate context rebuild, it's always up to date
    auto simulationTimeScale = simulationConfig_.setup.simulationTimeScale;
    auto simulationTimeStep = simulationConfig_.setup.simulationTimeStep;

    while (!simulationFactory_.getCell().getDiscs().empty())
    {
        if (QThread::currentThread()->isInterruptionRequested())
            break;

        timeSinceLastUpdate += clock.restart().asSeconds();

        while (timeSinceLastUpdate / simulationTimeScale > simulationTimeStep)
        {
            timeSinceLastUpdate -= simulationTimeStep / simulationTimeScale;
            simulationFactory_.getCell().update(simulationTimeStep);

            emitFrame(RedrawOnly{false});
        }
    }
}

void Simulation::buildContext(const cell::SimulationConfig& config)
{
    simulationFactory_.buildSimulationFromConfig(config);
}

void Simulation::rebuildContext()
{
    buildContext(simulationConfig_);
    notifyConfigObservers();
    emitFrame(RedrawOnly{true});
}

const cell::SimulationConfig& Simulation::getSimulationConfig() const
{
    return simulationConfig_;
}

void Simulation::setDiscTypes(const std::vector<cell::config::DiscType>& discTypes,
                              const std::unordered_set<std::string>& removedDiscTypes,
                              const std::map<std::string, sf::Color>& discTypeColorMap)
{
    SimulationConfigUpdater updater;
    updater.createDiscTypeChangeMap(discTypes, simulationConfig_.discTypes, removedDiscTypes);

    auto newConfig = simulationConfig_;
    newConfig.discTypes = discTypes;
    updater.removeDiscTypes(newConfig, removedDiscTypes);
    updater.updateDiscTypes(newConfig);

    buildContext(newConfig);

    // If the above line didn't throw, we're safe to actually change the config now

    simulationConfig_ = std::move(newConfig);
    discTypeColorMap_ = discTypeColorMap;

    notifyConfigObservers();
}

void Simulation::setSimulationConfig(const cell::SimulationConfig& simulationConfig)
{
    buildContext(simulationConfig);
    simulationConfig_ = simulationConfig;
    notifyConfigObservers();
}

const std::map<std::string, sf::Color>& Simulation::getDiscTypeColorMap() const
{
    return discTypeColorMap_;
}

void Simulation::setMembraneTypes(const std::vector<cell::config::MembraneType>& membraneTypes,
                                  const std::unordered_set<std::string>& removedMembraneTypes,
                                  const std::map<std::string, sf::Color>& membraneTypeColorMap)
{
    // TODO DRY violation with setDiscTypes
    SimulationConfigUpdater updater;
    updater.createMembraneTypeChangeMap(membraneTypes, simulationConfig_.membraneTypes, removedMembraneTypes);

    auto newConfig = simulationConfig_;
    newConfig.membraneTypes = membraneTypes;
    updater.removeMembraneTypes(newConfig, removedMembraneTypes);
    updater.updateMembraneTypes(newConfig);

    buildContext(newConfig);

    // If the above line didn't throw, we're safe to actually change the config now

    simulationConfig_ = std::move(newConfig);
    membraneTypeColorMap_ = membraneTypeColorMap;

    notifyConfigObservers();
}

const std::map<std::string, sf::Color>& Simulation::getMembraneTypeColorMap() const
{
    return membraneTypeColorMap_;
}

void Simulation::registerConfigObserver(ConfigObserver observer)
{
    configObservers_.push_back(std::move(observer));
}

const cell::DiscTypeRegistry& Simulation::getDiscTypeRegistry()
{
    return simulationFactory_.getSimulationContext().discTypeRegistry;
}

bool Simulation::cellIsBuilt() const
{
    return simulationFactory_.cellIsBuilt();
}

void Simulation::loadDefaultConfig()
{
    simulationConfig_ = {};
}

void Simulation::saveConfigToFile(const fs::path& path) const
{
    json j;
    j["config"] = simulationConfig_;
    j["discTypeColorMap"] = discTypeColorMap_;
    j["membraneTypeColorMap"] = membraneTypeColorMap_;

    std::ofstream file(path);
    file << j.dump(4);
}

void Simulation::loadConfigFromFile(const fs::path& path)
{
    json j;
    std::ifstream file(path);
    file >> j;

    simulationConfig_ = j["config"].get<cell::SimulationConfig>();
    discTypeColorMap_ = j["colorMap"].get<std::map<std::string, sf::Color>>();
    membraneTypeColorMap_ = j["membraneTypeColorMap"].get<std::map<std::string, sf::Color>>();

    rebuildContext();
}

void Simulation::notifyConfigObservers()
{
    for (const auto& observer : configObservers_)
        observer(simulationConfig_, discTypeColorMap_);

    emitFrame(RedrawOnly{true});
}

void Simulation::emitFrame(RedrawOnly redrawOnly)
{
    if (!simulationFactory_.cellIsBuilt())
        return;

    FrameDTO frameDTO;
    frameDTO.discs_ = simulationFactory_.getCell().getDiscs();

    if (redrawOnly.value)
    {
        emit frame(frameDTO);
        return;
    }

    frameDTO.elapsedSimulationTimeUs = static_cast<long long>(simulationConfig_.setup.simulationTimeStep * 1e6);
    frameDTO.collisionCounts_ = simulationFactory_.getAndResetCollisionCounts();

    emit frame(frameDTO);
}