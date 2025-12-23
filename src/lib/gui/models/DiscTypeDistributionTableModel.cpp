#include "models/DiscTypeDistributionTableModel.hpp"
#include "core/SimulationConfigUpdater.hpp"

#include <unordered_set>

DiscTypeDistributionTableModel::DiscTypeDistributionTableModel(QObject* parent,
                                                               SimulationConfigUpdater* simulationConfigUpdater)
    : AbstractSimulationConfigTableModel<DiscTypeDistributionEntry>(parent, {{"Disc type", "Frequency", "Delete"}},
                                                                    simulationConfigUpdater)
{
}

void DiscTypeDistributionTableModel::addRow()
{
    const auto& discTypes = simulationConfigUpdater_->getSimulationConfig().discTypes;
    std::unordered_set<std::string> availableNames;

    for (const auto& discType : discTypes)
        availableNames.insert(discType.name);

    for (const auto& [name, frequency] : rows_)
        availableNames.erase(name);

    if (availableNames.empty())
        throw ExceptionWithLocation("There are no disc types available that are not already in the distribution.");

    beginInsertRows(QModelIndex(), static_cast<int>(rows_.size()), static_cast<int>(rows_.size()));
    rows_.emplace_back(*availableNames.begin(), 0);
    endInsertRows();
}

QVariant DiscTypeDistributionTableModel::getField(const DiscTypeDistributionEntry& row, const QModelIndex& index) const
{
    switch (index.column())
    {
    case 0: return QString::fromStdString(row.first);
    case 1: return row.second;
    case 2: return "Delete";
    default: return {};
    }
}

bool DiscTypeDistributionTableModel::setField(DiscTypeDistributionEntry& row, const QModelIndex& index,
                                              const QVariant& value)
{
    switch (index.column())
    {
    case 0: row.first = value.toString().toStdString(); break;
    case 1: row.second = value.toDouble(); break;
    default: return false;
    }

    return true;
}

bool DiscTypeDistributionTableModel::isEditable(const QModelIndex& index) const
{
    return index.column() != 2;
}
