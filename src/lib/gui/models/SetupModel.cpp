#include "models/SetupModel.hpp"
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

void SetupModel::setReactionsConserveArea(bool value)
{
    simulationConfig_.reactionsConserveArea = value;
}

void SetupModel::setTimeStepUs(int timeStepUs)
{
    simulationConfig_.simulationTimeStep = timeStepUs * 1000LL;
}

void SetupModel::setTimeScale(double timeScale)
{
    simulationConfig_.simulationTimeScale = timeScale;
}

void SetupModel::setMostProbableSpeed(double mostProbableSpeed)
{
    simulationConfig_.mostProbableSpeed = mostProbableSpeed;
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
