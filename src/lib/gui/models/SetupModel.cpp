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

void SetupModel::commitChanges()
{
}

void SetupModel::discardChanges()
{
}