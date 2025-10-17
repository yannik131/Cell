#include "models/MembraneTypesTableModel.hpp"
#include "core/AbstractSimulationBuilder.hpp"
#include "core/ColorMapping.hpp"

// TODO Massive DRY violation here with the DiscTypesTableModel and possible other models, fix as soon as everything in
// the PR works

MembraneTypesTableModel::MembraneTypesTableModel(QObject* parent, AbstractSimulationBuilder* abstractSimulationBuilder)
    : QAbstractTableModel(parent)
    , abstractSimulationBuilder_(abstractSimulationBuilder)
{
}

int MembraneTypesTableModel::rowCount(const QModelIndex& parent) const
{
    return static_cast<int>(rows_.size());
}

int MembraneTypesTableModel::columnCount(const QModelIndex& parent) const
{
    return 5;
}

QVariant MembraneTypesTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal || section > columnCount() - 1)
        return {};

    static const QStringList Headers{"Name", "Radius", "Color", "Permeabilities", "Delete"};

    return Headers[section];
}

QVariant MembraneTypesTableModel::data(const QModelIndex& index, int role) const
{
    if (index.row() >= rows_.size() || (role != Qt::DisplayRole && role != Qt::EditRole))
        return {};

    const auto& membraneType = rows_.at(index.row());

    switch (index.column())
    {
    case 0:
        return QString::fromStdString(membraneType.name);
    case 1:
        return membraneType.radius;
    case 2:
        return "Edit";
    case 3:
        return getColorNameMapping()[membraneTypeColorMap_.at(membraneType.name)];
    case 4:
        return "Delete";
    default:
        return {};
    }
}

bool MembraneTypesTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (index.row() >= rows_.size() || role != Qt::EditRole)
        return false;

    auto& membraneType = rows_[index.row()];

    switch (index.column())
    {
    case 0:
        updateMembraneTypeName(value.toString().toStdString(), index.row());
        break;
    case 1:
        membraneType.radius = value.toDouble();
        break;
    case 2:
        membraneTypeColorMap_[membraneType.name] = getNameColorMapping()[value.toString()];
        break;
    default:
        return false;
    }

    if (index.column() < 4)
        emit dataChanged(index, index);

    return true;
}

Qt::ItemFlags MembraneTypesTableModel::flags(const QModelIndex& index) const
{
    return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void MembraneTypesTableModel::addEmptyRow()
{
    beginInsertRows(QModelIndex(), static_cast<int>(rows_.size()), static_cast<int>(rows_.size()));

    rows_.push_back(
        cell::config::MembraneType{.name = "Type" + std::to_string(rows_.size()), .radius = 1, .permeabilityMap = {}});
    membraneTypeColorMap_[rows_.back().name] = sf::Color::Blue;

    endInsertRows();
}

void MembraneTypesTableModel::removeRow(int row)
{
    if (row < 0 || row >= rows_.size())
        return;

    beginRemoveRows(QModelIndex(), row, row);
    membraneTypeColorMap_.erase(rows_[row].name);
    rows_.erase(rows_.begin() + row);
    endRemoveRows();

    const auto& originalMembraneTypes = abstractSimulationBuilder_->getSimulationConfig().membraneTypes;
    const auto originalIndex = row + removedMembraneTypes_.size();

    if (originalIndex < originalMembraneTypes.size())
        removedMembraneTypes_.insert(originalMembraneTypes[originalIndex].name);
}

void MembraneTypesTableModel::clearRows()
{
    if (rows_.empty())
        return;

    beginRemoveRows(QModelIndex(), 0, static_cast<int>(rows_.size()) - 1);
    rows_.clear();
    membraneTypeColorMap_.clear();
    endRemoveRows();

    for (const auto& membraneType : abstractSimulationBuilder_->getSimulationConfig().membraneTypes)
        removedMembraneTypes_.insert(membraneType.name);
}

void MembraneTypesTableModel::commitChanges()
{
    abstractSimulationBuilder_->setMembraneTypes(rows_, removedMembraneTypes_, membraneTypeColorMap_);

    removedMembraneTypes_.clear();
}

void MembraneTypesTableModel::reload()
{
    removedMembraneTypes_.clear();
    beginResetModel();
    rows_ = abstractSimulationBuilder_->getSimulationConfig().membraneTypes;
    membraneTypeColorMap_ = abstractSimulationBuilder_->getMembraneTypeColorMap();
    endResetModel();
}

void MembraneTypesTableModel::updateMembraneTypeName(const std::string& newName, int row)
{
    auto& membraneType = rows_[row];
    auto color = membraneTypeColorMap_.at(membraneType.name);
    membraneTypeColorMap_.erase(membraneType.name);

    membraneType.name = newName;
    membraneTypeColorMap_[newName] = color;
}
