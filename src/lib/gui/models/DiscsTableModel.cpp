#include "DiscsTableModel.hpp"
#include "core/AbstractSimulationBuilder.hpp"
#include "core/SimulationConfigUpdater.hpp"
#include "models/DiscsTableModel.hpp"

DiscsTableModel::DiscsTableModel(QObject* parent, SimulationConfigUpdater* simulationConfigUpdater)
    : AbstractSimulationConfigTableModel<cell::config::Disc>(parent, {{"Type", "x", "y", "vx", "vy", "Delete"}},
                                                             simulationConfigUpdater)
{
}

void DiscsTableModel::addRow()
{
    const auto& discTypes = simulationConfigUpdater_->getSimulationConfig().discTypes;
    if (discTypes.empty())
        throw ExceptionWithLocation("No disc types available. Define some first.");

    beginInsertRows(QModelIndex(), static_cast<int>(rows_.size()), static_cast<int>(rows_.size()));
    rows_.push_back(cell::config::Disc{.discTypeName = discTypes.front().name, .x = 0, .y = 0, .vx = 0, .vy = 0});
    endInsertRows();
}

void DiscsTableModel::loadFromConfig()
{
    setRows(simulationConfigUpdater_->getSimulationConfig().discs);
}

void DiscsTableModel::saveToConfig()
{
    auto currentConfig = simulationConfigUpdater_->getSimulationConfig();
    currentConfig.discs = rows_;
    simulationConfigUpdater_->setSimulationConfig(currentConfig);
}

QVariant DiscsTableModel::getField(const cell::config::Disc& row, int column) const
{
    switch (column)
    {
    case 0: return QString::fromStdString(row.discTypeName);
    case 1: return row.x;
    case 2: return row.y;
    case 3: return row.vx;
    case 4: return row.vy;
    case 5: return "Delete";
    default: return {};
    }
}

bool DiscsTableModel::setField(cell::config::Disc& row, int column, const QVariant& value)
{
    switch (column)
    {
    case 0: row.discTypeName = value.toString().toStdString(); break;
    case 1: row.x = value.toDouble(); break;
    case 2: row.y = value.toDouble(); break;
    case 3: row.vx = value.toDouble(); break;
    case 4: row.vy = value.toDouble(); break;
    default: return false;
    }

    return true;
}

bool DiscsTableModel::isEditable(const QModelIndex& index) const
{
    return index.column() != 5;
}
