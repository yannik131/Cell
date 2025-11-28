#include "core/Simulation.hpp"
#include "cell/Cell.hpp"
#include "cell/ExceptionWithLocation.hpp"
#include "core/SimulationConfigUpdater.hpp"

#include <QThread>
#include <QTimer>
#include <SFML/System/Clock.hpp>

#include <iostream>

Simulation::Simulation(QObject* parent)
    : QObject(parent)
{
}

void Simulation::run()
{
    sf::Clock clock;
    double timeSinceLastUpdate = 0;

    // Since every change to the config causes an immediate context rebuild, it's always up to date
    const auto& simulationConfig = simulationConfigUpdater_.getSimulationConfig();
    const auto simulationTimeScale = simulationConfig.simulationTimeScale;
    const auto simulationTimeStep = simulationConfig.simulationTimeStep;

    while (!QThread::currentThread()->isInterruptionRequested())
    {
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

const cell::DiscTypeRegistry& Simulation::getDiscTypeRegistry()
{
    return simulationFactory_.getSimulationContext().discTypeRegistry;
}

SimulationConfigUpdater& Simulation::getSimulationConfigUpdater()
{
    return simulationConfigUpdater_;
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
    const auto& cell = simulationFactory_.getCell();

    const auto gatherData = [&](const cell::Compartment& compartment)
    {
        frameDTO.discs_.insert(frameDTO.discs_.end(), compartment.getDiscs().begin(), compartment.getDiscs().end());
        frameDTO.membranes_.push_back(circleShapeFromCompartment(compartment));
    };

    std::vector<const cell::Compartment*> compartments({&cell});

    while (!compartments.empty())
    {
        const cell::Compartment* compartment = compartments.back();
        compartments.pop_back();

        gatherData(*compartment);

        for (const auto& subCompartment : compartment->getCompartments())
            compartments.push_back(subCompartment.get());
    }

    if (redrawOnly.value)
    {
        emit frame(frameDTO);
        return;
    }

    frameDTO.elapsedSimulationTimeUs =
        static_cast<long long>(simulationConfigUpdater_.getSimulationConfig().simulationTimeStep * 1e6);
    frameDTO.collisionCounts_ = simulationFactory_.getAndResetCollisionCounts();

    emit frame(frameDTO);
}

sf::CircleShape Simulation::circleShapeFromCompartment(const cell::Compartment& compartment)
{
    sf::CircleShape shape;
    const auto& membraneTypeRegistry = simulationFactory_.getSimulationContext().membraneTypeRegistry;
    const auto& membraneType = membraneTypeRegistry.getByID(compartment.getMembrane().getTypeID());
    const auto R = static_cast<float>(membraneType.getRadius());

    shape.setPointCount(100);
    shape.setRadius(R);
    shape.setOrigin(R, R);
    shape.setPosition(static_cast<sf::Vector2f>(compartment.getMembrane().getPosition()));
    shape.setFillColor(sf::Color::Transparent);
    shape.setOutlineThickness(1);

    if (membraneType.getName() == cell::config::cellMembraneTypeName)
        shape.setOutlineColor(sf::Color::Yellow);
    else
        shape.setOutlineColor(simulationConfigUpdater_.getMembraneTypeColorMap().at(membraneType.getName()));

    return shape;
}
