#include "core/Simulation.hpp"
#include "cell/Cell.hpp"
#include "cell/CollisionDetector.hpp"
#include "cell/ExceptionWithLocation.hpp"
#include "core/SimulationConfigUpdater.hpp"
#include "core/Utility.hpp"

#include "Simulation.hpp"
#include <QThread>
#include <QTimer>

Simulation::Simulation(QObject* parent)
    : QObject(parent)
{
}

void Simulation::start()
{
    simulationRunner_.runSimulation();
}

void Simulation::stop()
{
    simulationRunner_.stopSimulation();
}

void Simulation::reinitialize()
{
    simulationRunner_.useConfig(simulationConfigUpdater_.getSimulationConfig());
    initializeSimulationRecorder();
}

void Simulation::loadSettingsFromJson(const fs::path& settingsPath)
{
    simulationConfigUpdater_.loadConfigFromFile(settingsPath);
    reinitialize();
}

const cell::DiscTypeRegistry& Simulation::getDiscTypeRegistry()
{
    return simulationFactory_.getSimulationContext().discTypeRegistry;
}

SimulationConfigUpdater& Simulation::getSimulationConfigUpdater()
{
    return simulationConfigUpdater_;
}

const cell::SimulationConfig& Simulation::getSimulationConfig() const
{
    return simulationConfigUpdater_.getSimulationConfig();
}

void Simulation::initializeSimulationRecorder()
{
    simulationRecorder_ =
        std::make_unique<cell::SimulationRecorder>(simulationRunner_.getSimulationContext().discTypeRegistry,
                                                   simulationRunner_.getSimulationConfig().mostProbableSpeed);
    simulationRunner_.setPerformanceDataCallback([&](auto data) { emit performanceData(data); });
    simulationRunner_.setPostBuildCallback(
        [&](cell::Cell& cell)
        {
            simulationRecorder_->processInitialSimulationData(cell);
            emit frame(simulationRecorder_->getLastFrame());
        });
    simulationRunner_.setPostUpdateCallback(
        [&](cell::Cell& cell, const ch::duration<double>& elapsedTime)
        {
            simulationRecorder_->processSimulationData(cell, elapsedTime);
            emit frame(simulationRecorder_->getLastFrame());
        });
    simulationRecorder_->setNewDataPointCallback([&](const cell::DataPoint& dataPoint)
                                                 { emit this->dataPoint(dataPoint); });
}

sf::CircleShape Simulation::circleShapeFromCompartment(const cell::Compartment& compartment)
{
    sf::CircleShape shape;
    const auto& membraneTypeRegistry = simulationFactory_.getSimulationContext().membraneTypeRegistry;
    const auto& membraneType = membraneTypeRegistry.getByID(compartment.getMembrane().getTypeID());
    const auto R = static_cast<float>(membraneType.getRadius());

    shape.setPointCount(100);
    shape.setRadius(R);
    shape.setOrigin({R, R});
    shape.setPosition(utility::toVector2f(compartment.getMembrane().getPosition()));
    shape.setFillColor(sf::Color::Transparent);
    shape.setOutlineThickness(1);

    if (membraneType.getName() == cell::config::cellMembraneTypeName)
        shape.setOutlineColor(sf::Color::Yellow);
    else
        shape.setOutlineColor(simulationConfigUpdater_.getMembraneTypeColorMap().at(membraneType.getName()));

    return shape;
}
