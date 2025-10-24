#include "models/DiscTypesTableModel.hpp"
#include "core/AbstractSimulationBuilder.hpp"
#include "core/ColorMapping.hpp"
#include "core/SimulationConfigUpdater.hpp"

#include "DiscTypesTableModel.hpp"
#include <set>

DiscTypesTableModel::DiscTypesTableModel(QObject* parent, SimulationConfigUpdater* simulationConfigUpdater)
    : Base(parent, {{"Name", "Radius", "Mass", "Color", "Delete"}}, simulationConfigUpdater)
{
}

void DiscTypesTableModel::removeRow(int row)

{
    Base::removeRow(row);
    discTypeColorMap_.erase(rows_[row].name);

    const auto& originalDiscTypes = simulationConfigUpdater_->getSimulationConfig().discTypes;
    const auto originalIndex = row + removedDiscTypes_.size();

    if (originalIndex < originalDiscTypes.size())
        removedDiscTypes_.insert(originalDiscTypes[originalIndex].name);
}

void DiscTypesTableModel::clearRows()
{
    Base::clearRows();
    discTypeColorMap_.clear();

    for (const auto& discType : simulationConfigUpdater_->getSimulationConfig().discTypes)
        removedDiscTypes_.insert(discType.name);
}

void DiscTypesTableModel::addRow()
{
    beginInsertRows(QModelIndex(), static_cast<int>(rows_.size()), static_cast<int>(rows_.size()));

    rows_.push_back(cell::config::DiscType{.name = "Type" + std::to_string(rows_.size()), .radius = 1, .mass = 1});
    discTypeColorMap_[rows_.back().name] = sf::Color::Blue;

    endInsertRows();
}

void DiscTypesTableModel::loadFromConfig()
{
    removedDiscTypes_.clear();

    beginResetModel();
    rows_ = simulationConfigUpdater_->getSimulationConfig().discTypes;
    discTypeColorMap_ = simulationConfigUpdater_->getDiscTypeColorMap();
    endResetModel();
}

void DiscTypesTableModel::saveToConfig()
{
    auto currentConfig = simulationConfigUpdater_->getSimulationConfig();
    simulationConfigUpdater_->setTypes(rows_, removedDiscTypes_, discTypeColorMap_);

    removedDiscTypes_.clear();
}

QVariant DiscTypesTableModel::getField(const cell::config::DiscType& row, int column) const
{
    switch (column)
    {
    case 0: return QString::fromStdString(row.name);
    case 1: return row.radius;
    case 2: return row.mass;
    case 3: return getColorNameMapping()[discTypeColorMap_.at(row.name)];
    case 4: return "Delete";
    default: return {};
    }
}

bool DiscTypesTableModel::setField(cell::config::DiscType& row, int column, const QVariant& value)
{
    switch (column)
    {
    case 0: updateDiscTypeName(row, value.toString().toStdString()); break;
    case 1: row.radius = value.toDouble(); break;
    case 2: row.mass = value.toDouble(); break;
    case 3: discTypeColorMap_[row.name] = getNameColorMapping()[value.toString()]; break;
    default: return false;
    }

    return true;
}

bool DiscTypesTableModel::isEditable(const QModelIndex& index) const
{
    return true;
}

void DiscTypesTableModel::updateDiscTypeName(cell::config::DiscType& discType, const std::string& newName)
{
    auto color = discTypeColorMap_.at(discType.name);
    discTypeColorMap_.erase(discType.name);

    discType.name = newName;
    discTypeColorMap_[newName] = color;
}