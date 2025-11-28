#include "models/SetupModel.hpp"
#include "SetupModel.hpp"
#include "core/SimulationConfigUpdater.hpp"
#include "models/DiscTypeDistributionTableModel.hpp"
#include "models/DiscsTableModel.hpp"

SetupModel::SetupModel(QObject* parent, SimulationConfigUpdater* simulationConfigUpdater)
    : QObject(parent)
    , simulationConfigUpdater_(simulationConfigUpdater)
{
}

void SetupModel::setUseDistribution(bool useDistribution)
{
    simulationConfig_.useDistribution = useDistribution;
}

void SetupModel::setTimeStepUs(int timeStepUs)
{
    simulationConfig_.simulationTimeStep = timeStepUs / 1e6;
}

void SetupModel::setTimeScale(double timeScale)
{
    simulationConfig_.simulationTimeScale = timeScale;
}

void SetupModel::setMaxVelocity(int maxVelocity)
{
    simulationConfig_.maxVelocity = static_cast<double>(maxVelocity);
}

void SetupModel::setFPS(int FPS)
{
    simulationConfigUpdater_->setFPS(FPS);
}

void SetupModel::saveToConfig()
{
    simulationConfigUpdater_->setSimulationConfig(simulationConfig_);
}

void SetupModel::loadFromConfig()
{
    simulationConfig_ = simulationConfigUpdater_->getSimulationConfig();
}
