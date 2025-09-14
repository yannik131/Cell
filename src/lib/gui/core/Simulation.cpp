#include "core/Simulation.hpp"
#include "cell/ExceptionWithLocation.hpp"
#include "cell/SFMLJsonSerializers.hpp"
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
    sf::Time timeSinceLastUpdate;

    // Since every change to the config causes an immediate context rebuild, it's always up to date
    auto simulationTimeScale = static_cast<float>(simulationConfig_.setup.simulationTimeScale);
    auto simulationTimeStep = sf::seconds(static_cast<float>(simulationConfig_.setup.simulationTimeStep));

    while (!simulationContext_.getCell().getDiscs().empty())
    {
        if (QThread::currentThread()->isInterruptionRequested())
            break;

        timeSinceLastUpdate += clock.restart();

        while (timeSinceLastUpdate / simulationTimeScale > simulationTimeStep)
        {
            timeSinceLastUpdate -= simulationTimeStep / simulationTimeScale;
            simulationContext_.getCell().update();

            emitFrame(RedrawOnly{false});
        }
    }
}

void Simulation::buildContext(const cell::SimulationConfig& config)
{
    simulationContext_.buildContextFromConfig(config);
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
    auto changeMap = updater.createChangeMap(discTypes, simulationConfig_.discTypes, removedDiscTypes);

    auto newConfig = simulationConfig_;
    newConfig.discTypes = discTypes;
    updater.removeDiscTypes(newConfig, removedDiscTypes);
    updater.updateDiscTypes(newConfig, changeMap);

    discTypeColorMap_ = discTypeColorMap;
    buildContext(newConfig);

    simulationConfig_ = std::move(newConfig);

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

void Simulation::registerConfigObserver(ConfigObserver observer)
{
    configObservers_.push_back(std::move(observer));
}

cell::DiscTypeResolver Simulation::getDiscTypeResolver() const
{
    return simulationContext_.getDiscTypeRegistry().getDiscTypeResolver();
}

bool Simulation::contextIsBuilt() const
{
    return simulationContext_.isBuilt();
}

void Simulation::loadDefaultConfig()
{
    simulationConfig_ = {};
}

void Simulation::saveConfigToFile(const fs::path& path) const
{
    json j;
    j["config"] = simulationConfig_;
    j["colorMap"] = discTypeColorMap_;

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
    if (!simulationContext_.isBuilt())
        return;

    FrameDTO frameDTO;
    frameDTO.discs_ = simulationContext_.getCell().getDiscs();

    if (!redrawOnly.value)
        frameDTO.elapsedSimulationTimeUs = simulationConfig_.setup.simulationTimeStep * 1e6;

    emit frame(frameDTO);
}
