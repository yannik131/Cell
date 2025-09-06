#include "core/Simulation.hpp"
#include "cell/ExceptionWithLocation.hpp"

#include <QThread>
#include <SFML/System/Clock.hpp>

#include "Simulation.hpp"
#include <glog/logging.h>

Simulation::Simulation(QObject* parent)
    : QObject(parent)
{
}

void Simulation::run()
{
    sf::Clock clock;
    sf::Time timeSinceLastUpdate;

    // Since every change to the config causes an immediate context rebuild, it's always up to date
    auto simulationTimeScale = static_cast<float>(simulationConfig_.setup.simulationTimeScale);
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

void Simulation::setSimulationConfig(const cell::SimulationConfig& simulationConfig)
{
    simulationContext_.buildContextFromConfig(simulationConfig);
    simulationConfig_ = simulationConfig;

    notifyDiscTypeObservers();
}

void Simulation::setDiscTypeColorMap(const std::map<std::string, sf::Color>& discTypeColorMap)
{
    discTypeColorMap_ = discTypeColorMap;
}

const std::map<std::string, sf::Color>& Simulation::getDiscTypeColorMap() const
{
    return discTypeColorMap_;
}

void Simulation::registerDiscTypeObserver(DiscTypeObserver observer)
{
    observers_.push_back(std::move(observer));
}

void Simulation::notifyDiscTypeObservers()
{
    for (auto iter = observers_.begin(); iter != observers_.end();)
    {
        if ((*iter)(simulationConfig_.discTypes))
            ++iter;
        else
            iter = observers_.erase(iter);
    }
}