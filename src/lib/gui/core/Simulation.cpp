#include "core/Simulation.hpp"
#include "cell/ExceptionWithLocation.hpp"

#include <QThread>
#include <SFML/System/Clock.hpp>

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

            simulationContext.getCell().update();
            emitFrameData();
        }
    }
}

void Simulation::buildContext()
{
    tryBuildContext(true);
}

void Simulation::emitFrameData(bool noTimeElapsed)
{
    // TODO
}

void Simulation::receiveDiscTypes(const std::vector<cell::config::DiscType>& discTypes)
{
    simulationConfig_.discTypes = discTypes;
    tryBuildContext();
}

void Simulation::receiveReactions(const std::vector<cell::config::Reaction>& reactions)
{
    simulationConfig_.reactions = reactions;
    tryBuildContext();
}

void Simulation::receiveSetup(const cell::config::Setup& setup)
{
    simulationConfig_.setup = setup;
    setupReceived_ = true;

    tryBuildContext();
}

void Simulation::tryBuildContext(bool throwIfIncomplete)
{
    if (simulationConfig_.discTypes.empty() || !setupReceived_)
    {
        if (throwIfIncomplete)
            throw ExceptionWithLocation(
                "Can't build simulation context: Incomplete setup (disc types or setup information missing)");
        else
            return;
    }

    simulationContext.buildContextFromConfig(simulationConfig_);
}
