#include "models/DiscTypesTableModel.hpp"
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
    const auto name = rows_[row].name;
    Base::removeRow(row);
    discColors_.erase(discColors_.begin() + row);

    const auto& originalDiscTypes = simulationConfigUpdater_->getSimulationConfig().discTypes;
    const auto originalIndex = row + removedDiscTypes_.size();

    if (originalIndex < originalDiscTypes.size())
        removedDiscTypes_.insert(originalDiscTypes[originalIndex].name);
}

void DiscTypesTableModel::clearRows()
{
    Base::clearRows();
    discColors_.clear();

    for (const auto& discType : simulationConfigUpdater_->getSimulationConfig().discTypes)
        removedDiscTypes_.insert(discType.name);
}

void DiscTypesTableModel::addRow()
{
    beginInsertRows(QModelIndex(), static_cast<int>(rows_.size()), static_cast<int>(rows_.size()));

    rows_.push_back(cell::config::DiscType{.name = "Type" + std::to_string(rows_.size()), .radius = 1, .mass = 1});
    discColors_.push_back(getSupportedDiscColors()[discColors_.size() % getSupportedDiscColors().size()]);

    endInsertRows();
}

void DiscTypesTableModel::loadFromConfig()
{
    removedDiscTypes_.clear();
    discColors_.clear();

    beginResetModel();
    rows_ = simulationConfigUpdater_->getSimulationConfig().discTypes;
    for (const auto& row : rows_)
        discColors_.push_back(simulationConfigUpdater_->getDiscTypeColorMap().at(row.name));
    endResetModel();
}

void DiscTypesTableModel::saveToConfig()
{
    auto currentConfig = simulationConfigUpdater_->getSimulationConfig();
    std::map<std::string, sf::Color> discTypeColorMap;
    for (std::size_t i = 0; i < discColors_.size(); ++i)
        discTypeColorMap[rows_[i].name] = discColors_[i];
    simulationConfigUpdater_->setTypes(rows_, removedDiscTypes_, discTypeColorMap);

    removedDiscTypes_.clear();
}

QVariant DiscTypesTableModel::getField(const cell::config::DiscType& row, const QModelIndex& index) const
{
    switch (index.column())
    {
    case 0: return QString::fromStdString(row.name);
    case 1: return row.radius;
    case 2: return row.mass;
    case 3: return getColorNameMapping()[discColors_[index.row()]];
    case 4: return "Delete";
    default: return {};
    }
}

bool DiscTypesTableModel::setField(cell::config::DiscType& row, const QModelIndex& index, const QVariant& value)
{
    switch (index.column())
    {
    case 0: row.name = value.toString().toStdString(); break;
    case 1: row.radius = value.toDouble(); break;
    case 2: row.mass = value.toDouble(); break;
    case 3: discColors_[index.row()] = getNameColorMapping()[value.toString()]; break;
    default: return false;
    }

    return true;
}

bool DiscTypesTableModel::isEditable(const QModelIndex& index) const
{
    return index.column() != 4;
}