#include "models/PermeabilityTableModel.hpp"
#include "core/PermeabilityMapping.hpp"
#include "core/SimulationConfigUpdater.hpp"

#include "PermeabilityTableModel.hpp"
#include <unordered_set>

// TODO DRY violation with DiscTypeDistributionTableModel

PermeabilityTableModel::PermeabilityTableModel(QObject* parent, SimulationConfigUpdater* simulationConfigUpdater)
    : AbstractSimulationConfigTableModel<PermeabilityMapEntry>(parent, {{"Disc type", "Permeability", "Delete"}},
                                                               simulationConfigUpdater)
{
}

void PermeabilityTableModel::addRow()
{
    const auto& discTypes = simulationConfigUpdater_->getSimulationConfig().discTypes;
    std::unordered_set<std::string> availableNames;

    for (const auto& discType : discTypes)
        availableNames.insert(discType.name);

    for (const auto& [name, frequency] : rows_)
        availableNames.erase(name);

    if (availableNames.empty())
        throw ExceptionWithLocation("There are no remaining disc types available.");

    beginInsertRows(QModelIndex(), static_cast<int>(rows_.size()), static_cast<int>(rows_.size()));
    rows_.emplace_back(*availableNames.begin(), 0);
    endInsertRows();
}

QVariant PermeabilityTableModel::getField(const PermeabilityMapEntry& row, int column) const
{
    switch (column)
    {
    case 0: return QString::fromStdString(row.first);
    case 1: return getPermeabilityNameMapping()[row.second];
    case 2: return "Delete";
    default: return {};
    }
}

bool PermeabilityTableModel::setField(PermeabilityMapEntry& row, int column, const QVariant& value)
{
    switch (column)
    {
    case 0:
        auto tmp = row.second;
        row.first = value.toString().toStdString();
        break;
    case 1: row.second = getNamePermeabilityMapping()[value.toString()]; break;
    default: return false;
    }

    return true;
}

bool PermeabilityTableModel::isEditable(const QModelIndex& index) const
{
    return index.column() != 2;
}
