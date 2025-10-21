#include "core/Simulation.hpp"
#include "cell/Cell.hpp"
#include "cell/ExceptionWithLocation.hpp"
#include "core/SimulationConfigUpdater.hpp"

#include <QThread>
#include <QTimer>
#include <SFML/System/Clock.hpp>

Simulation::Simulation(QObject* parent)
    : QObject(parent)
{
}

void Simulation::run()
{
    sf::Clock clock;
    double timeSinceLastUpdate;

    // Since every change to the config causes an immediate context rebuild, it's always up to date
    const auto& simulationConfig = simulationConfigUpdater_.getSimulationConfig();
    const auto simulationTimeScale = simulationConfig.setup.simulationTimeScale;
    const auto simulationTimeStep = simulationConfig.setup.simulationTimeStep;

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
    buildContext(simulationConfigUpdater_.getSimulationConfig());
    emitFrame(RedrawOnly{true});
}

const cell::SimulationConfig& Simulation::getSimulationConfig() const
{
    return simulationConfigUpdater_.getSimulationConfig();
}

const cell::DiscTypeRegistry& Simulation::getDiscTypeRegistry()
{
    return simulationFactory_.getSimulationContext().discTypeRegistry;
}

bool Simulation::cellIsBuilt() const
{
    return simulationFactory_.cellIsBuilt();
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

    frameDTO.elapsedSimulationTimeUs =
        static_cast<long long>(simulationConfigUpdater_.getSimulationConfig().setup.simulationTimeStep * 1e6);
    frameDTO.collisionCounts_ = simulationFactory_.getAndResetCollisionCounts();

    emit frame(frameDTO);
}