#include "models/MembraneTypesTableModel.hpp"
#include "MembraneTypesTableModel.hpp"
#include "core/ColorMapping.hpp"
#include "core/SimulationConfigUpdater.hpp"

// TODO DRY violation with DiscTypesTableModel

MembraneTypesTableModel::MembraneTypesTableModel(QObject* parent, SimulationConfigUpdater* simulationConfigUpdater)
    : Base(parent, {{"Name", "Radius", "Color", "Permeabilities", "Distribution", "Disc count", "Delete"}},
           simulationConfigUpdater)
{
}

void MembraneTypesTableModel::removeRow(int row)
{
    const auto name = rows_[row].name;
    Base::removeRow(row);
    membraneTypeColorMap_.erase(name);

    const auto& originalMembraneTypes = simulationConfigUpdater_->getSimulationConfig().membraneTypes;
    const auto originalIndex = row - 1 + removedMembraneTypes_.size(); // - 1 because the first row is the cell membrane

    if (originalIndex < originalMembraneTypes.size())
        removedMembraneTypes_.insert(originalMembraneTypes[originalIndex].name);
}

void MembraneTypesTableModel::clearRows()
{
    Base::clearRows();
    membraneTypeColorMap_.clear();

    for (const auto& membraneType : simulationConfigUpdater_->getSimulationConfig().membraneTypes)
        removedMembraneTypes_.insert(membraneType.name);

    beginInsertRows(QModelIndex(), 0, 0);
    rows_.push_back(simulationConfigUpdater_->getSimulationConfig().cellMembraneType);
    membraneTypeColorMap_[cell::config::cellMembraneTypeName] = sf::Color::Yellow;
    endInsertRows();
}

void MembraneTypesTableModel::addRow()
{
    beginInsertRows(QModelIndex(), static_cast<int>(rows_.size()), static_cast<int>(rows_.size()));

    rows_.push_back(cell::config::MembraneType{
        .name = "Type" + std::to_string(rows_.size()), .radius = 1, .permeabilityMap = {}, .discCount = 0});
    membraneTypeColorMap_[rows_.back().name] = sf::Color::Blue;

    endInsertRows();
}

void MembraneTypesTableModel::loadFromConfig()
{
    removedMembraneTypes_.clear();

    const auto& config = simulationConfigUpdater_->getSimulationConfig();

    beginResetModel();
    rows_ = {config.cellMembraneType};
    rows_.insert(rows_.end(), config.membraneTypes.begin(), config.membraneTypes.end());
    membraneTypeColorMap_ = simulationConfigUpdater_->getMembraneTypeColorMap();
    membraneTypeColorMap_[cell::config::cellMembraneTypeName] = sf::Color::Yellow;
    endResetModel();
}

void MembraneTypesTableModel::saveToConfig()
{
    // Set cell membrane separately
    auto config = simulationConfigUpdater_->getSimulationConfig();
    config.cellMembraneType = rows_[0];
    simulationConfigUpdater_->setSimulationConfig(config);

    // Remove cell membrane from list
    auto rows = rows_;
    rows.erase(rows.begin());
    simulationConfigUpdater_->setTypes(rows, removedMembraneTypes_, membraneTypeColorMap_);

    removedMembraneTypes_.clear();
}

QVariant MembraneTypesTableModel::getField(const cell::config::MembraneType& row, int column) const
{
    switch (column)
    {
    case 0: return QString::fromStdString(row.name);
    case 1: return row.radius;
    case 2: return getColorNameMapping()[membraneTypeColorMap_.at(row.name)];
    case 3:
    case 4: return "Edit";
    case 5: return row.discCount;
    case 6: return "Delete";
    default: return {};
    }
}

bool MembraneTypesTableModel::setField(cell::config::MembraneType& row, int column, const QVariant& value)
{
    switch (column)
    {
    case 0: updateMembraneTypeName(row, value.toString().toStdString()); break;
    case 1: row.radius = value.toDouble(); break;
    case 2: membraneTypeColorMap_[row.name] = getNameColorMapping()[value.toString()]; break;
    case 5: row.discCount = value.toInt(); break;
    default: return false;
    }

    return true;
}

bool MembraneTypesTableModel::isEditable(const QModelIndex& index) const
{
    return index.column() != 6;
}

bool MembraneTypesTableModel::isEnabled(const QModelIndex& index) const
{
    if (index.row() == 0)
        return index.column() != 0 && index.column() != 3 && index.column() != 6;

    return true;
}

void MembraneTypesTableModel::updateMembraneTypeName(cell::config::MembraneType& membraneType,
                                                     const std::string& newName)
{
    auto color = membraneTypeColorMap_.at(membraneType.name);
    membraneTypeColorMap_.erase(membraneType.name);

    membraneType.name = newName;
    membraneTypeColorMap_[newName] = color;
}
