#include "core/Simulation.hpp"
#include "cell/ExceptionWithLocation.hpp"
#include "core/SimulationConfigUpdater.hpp"

#include "Simulation.hpp"
#include <QThread>
#include <SFML/System/Clock.hpp>

Simulation::Simulation(QObject* parent)
    : QObject(parent)
{
}

void Simulation::run()
{
    sf::Clock clock;
    sf::Time timeSinceLastUpdate;

    // Since every change to the config causes an immediate context rebuild, it's always up to date
    auto simulationTimeScale = 1.0f;
    auto simulationTimeStep = sf::seconds(static_cast<float>(simulationConfig_.setup.simulationTimeStep));

    while (true)
    {
        if (QThread::currentThread()->isInterruptionRequested())
            break;

        timeSinceLastUpdate += clock.restart();

        while (timeSinceLastUpdate / simulationTimeScale > simulationTimeStep)
        {
            timeSinceLastUpdate -= simulationTimeStep / simulationTimeScale;

            simulationContext_.getCell().update();
            emitFrameData();
        }
    }
}

void Simulation::buildContext()
{
    simulationContext_.buildContextFromConfig(simulationConfig_);
}

void Simulation::emitFrameData(bool noTimeElapsed)
{
    // TODO
}

const cell::SimulationConfig& Simulation::getSimulationConfig() const
{
    return simulationConfig_;
}

void Simulation::setDiscTypes(const std::vector<cell::config::DiscType>& discTypes,
                              const std::unordered_set<std::string>& removedDiscTypes)
{
    SimulationConfigUpdater updater;
    auto changeMap = updater.createChangeMap(discTypes, simulationConfig_.discTypes, removedDiscTypes);

    auto newConfig = simulationConfig_;
    newConfig.discTypes = discTypes;
    updater.removeDiscTypes(newConfig, removedDiscTypes);
    updater.updateDiscTypes(newConfig, changeMap);

    simulationContext_.buildContextFromConfig(newConfig);

    simulationConfig_ = std::move(newConfig);
    notifyConfigObservers();
}

void Simulation::setSimulationConfig(const cell::SimulationConfig& simulationConfig)
{
    simulationContext_.buildContextFromConfig(simulationConfig);
    simulationConfig_ = simulationConfig;
}

void Simulation::setDiscTypeColorMap(const std::map<std::string, sf::Color>& discTypeColorMap)
{
    discTypeColorMap_ = discTypeColorMap;
}

const std::map<std::string, sf::Color>& Simulation::getDiscTypeColorMap() const
{
    return discTypeColorMap_;
}

void Simulation::registerConfigObserver(ConfigObserver observer)
{
    observers_.push_back(std::move(observer));
}

void Simulation::notifyConfigObservers()
{
    for (const auto& observer : observers_)
        observer(simulationConfig_);
}