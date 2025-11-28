#include "models/MembranesTableModel.hpp"
#include "MembranesTableModel.hpp"
#include "core/SimulationConfigUpdater.hpp"

MembranesTableModel::MembranesTableModel(QObject* parent, SimulationConfigUpdater* simulationConfigUpdater)
    : AbstractSimulationConfigTableModel<cell::config::Membrane>(parent, {{"Membrane type", "x", "y", "Delete"}},
                                                                 simulationConfigUpdater)
{
}

void MembranesTableModel::addRow()
{
    const auto& membraneTypes = simulationConfigUpdater_->getSimulationConfig().membraneTypes;
    if (membraneTypes.empty())
        throw ExceptionWithLocation("No membrane types available. Define some first.");

    beginInsertRows(QModelIndex(), static_cast<int>(rows_.size()), static_cast<int>(rows_.size()));
    rows_.push_back(cell::config::Membrane{.membraneTypeName = membraneTypes.front().name, .x = 0, .y = 0});
    endInsertRows();
}

void MembranesTableModel::loadFromConfig()
{
    setRows(simulationConfigUpdater_->getSimulationConfig().membranes);
}

void MembranesTableModel::saveToConfig()
{
    auto currentConfig = simulationConfigUpdater_->getSimulationConfig();
    currentConfig.membranes = rows_;
    simulationConfigUpdater_->setSimulationConfig(currentConfig);
}

QVariant MembranesTableModel::getField(const cell::config::Membrane& row, int column) const
{
    switch (column)
    {
    case 0: return QString::fromStdString(row.membraneTypeName);
    case 1: return row.x;
    case 2: return row.y;
    case 3: return "Delete";
    default: return {};
    }
}

bool MembranesTableModel::setField(cell::config::Membrane& row, int column, const QVariant& value)
{
    switch (column)
    {
    case 0: row.membraneTypeName = value.toString().toStdString(); break;
    case 1: row.x = value.toDouble(); break;
    case 2: row.y = value.toDouble(); break;
    default: return false;
    }

    return true;
}

bool MembranesTableModel::isEditable(const QModelIndex& index) const
{
    return index.column() != 3;
}
