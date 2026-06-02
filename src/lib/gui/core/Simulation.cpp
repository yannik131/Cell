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
    simulationRunner_.setPostStartCallback([&]() { emit started(); });
    simulationRunner_.setPostStartCallback([&]() { emit stopped(); });
}

void Simulation::start()
{
    simulationRunner_.runSimulation();
}

void Simulation::stop()
{
    simulationRunner_.stopSimulation();
}

bool Simulation::isRunning() const
{
    return simulationRunner_.simulationIsRunning();
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

void Simulation::emitLastFrame()
{
    emit frame(simulationRecorder_->getLastFrame());
}

SimulationConfigUpdater& Simulation::getSimulationConfigUpdater()
{
    return simulationConfigUpdater_;
}

const cell::SimulationConfig& Simulation::getSimulationConfig() const
{
    return simulationConfigUpdater_.getSimulationConfig();
}

cell::SimulationContext Simulation::getSimulationContext()
{
    return simulationRunner_.getSimulationContext();
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
