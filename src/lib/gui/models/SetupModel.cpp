#include "models/SetupModel.hpp"
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

void SetupModel::setTimeStep(int timeStep)
{
    setup_.simulationTimeStep = timeStep;
}

void SetupModel::setTimeScale(int timeScale)
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

void SetupModel::setGUIFPS(int GUIFPS)
{
    // TODO
}

void SetupModel::commitChanges()
{
    auto config = abstractSimulationBuilder_->getSimulationConfig();
}

void SetupModel::discardChanges()
{
}