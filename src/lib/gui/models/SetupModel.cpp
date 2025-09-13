#include "models/SetupModel.hpp"
#include "SetupModel.hpp"
#include "core/AbstractSimulationBuilder.hpp"
#include "models/DiscTableModel.hpp"
#include "models/DiscTypeDistributionTableModel.hpp"

SetupModel::SetupModel(QObject* parent, DiscTypeDistributionTableModel* discTypeDistributionTableModel,
                       DiscTableModel* discTableModel, AbstractSimulationBuilder* abstractSimulationBuilder)
    : QObject(parent)
    , discTypeDistributionTableModel_(discTypeDistributionTableModel)
    , discTableModel_(discTableModel)
    , abstractSimulationBuilder_(abstractSimulationBuilder)
{
}

void SetupModel::setUseDistribution(bool useDistribution)
{
    setup_.useDistribution = useDistribution;
}

void SetupModel::setNumberOfDiscs(int numberOfDiscs)
{
    setup_.discCount = numberOfDiscs;
}

void SetupModel::setTimeStepUs(int timeStepUs)
{
    setup_.simulationTimeStep = timeStepUs / 1e6;
}

void SetupModel::setTimeScale(double timeScale)
{
    setup_.simulationTimeScale = timeScale;
}

void SetupModel::setCellWidth(int cellWidth)
{

    setup_.cellWidth = cellWidth;
}

void SetupModel::setCellHeight(int cellHeight)
{
    setup_.cellHeight = cellHeight;
}

void SetupModel::commitChanges()
{
    loadDiscsAndDistribution();

    auto config = abstractSimulationBuilder_->getSimulationConfig();
    config.setup = setup_;

    abstractSimulationBuilder_->setSimulationConfig(config);
}

void SetupModel::discardChanges()
{
    setup_ = abstractSimulationBuilder_->getSimulationConfig().setup;
}

const cell::config::Setup& SetupModel::getSetup()
{
    loadDiscsAndDistribution();
    return setup_;
}

void SetupModel::loadDiscsAndDistribution()
{
    const auto& distributionEntries = discTypeDistributionTableModel_->getRows();
    setup_.distribution = std::map<std::string, double>(distributionEntries.begin(), distributionEntries.end());

    setup_.discs = discTableModel_->getRows();
}
