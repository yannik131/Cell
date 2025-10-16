#include "models/DiscTypesTableModel.hpp"
#include "core/AbstractSimulationBuilder.hpp"
#include "core/ColorMapping.hpp"

#include "DiscTypesTableModel.hpp"
#include <set>

DiscTypesTableModel::DiscTypesTableModel(QObject* parent, AbstractSimulationBuilder* abstractSimulationBuilder)
    : QAbstractTableModel(parent)
    , abstractSimulationBuilder_(abstractSimulationBuilder)
{
}

int DiscTypesTableModel::rowCount(const QModelIndex&) const
{
    return static_cast<int>(rows_.size());
}

int DiscTypesTableModel::columnCount(const QModelIndex&) const
{
    return 5;
}

QVariant DiscTypesTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal || section > columnCount() - 1)
        return {};

    static const QStringList Headers{"Name", "Radius", "Mass", "Color", "Delete"};

    return Headers[section];
}

QVariant DiscTypesTableModel::data(const QModelIndex& index, int role) const
{
    if (index.row() >= rows_.size() || (role != Qt::DisplayRole && role != Qt::EditRole))
        return {};

    const auto& discType = rows_.at(index.row());

    switch (index.column())
    {
    case 0:
        return QString::fromStdString(discType.name);
    case 1:
        return discType.radius;
    case 2:
        return discType.mass;
    case 3:
        return getColorNameMapping()[discTypeColorMap_.at(discType.name)];
    case 4:
        return "Delete";
    default:
        return {};
    }
}

bool DiscTypesTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (index.row() >= rows_.size() || role != Qt::EditRole)
        return false;

    auto& discType = rows_[index.row()];

    switch (index.column())
    {
    case 0:
        updateDiscTypeName(value.toString().toStdString(), index.row());
        break;
    case 1:
        discType.radius = value.toDouble();
        break;
    case 2:
        discType.mass = value.toDouble();
        break;
    case 3:
        discTypeColorMap_[discType.name] = getNameColorMapping()[value.toString()];
        break;
    default:
        return false;
    }

    if (index.column() < 4)
        emit dataChanged(index, index);

    return true;
}

Qt::ItemFlags DiscTypesTableModel::flags(const QModelIndex&) const
{
    return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void DiscTypesTableModel::addEmptyRow()
{
    beginInsertRows(QModelIndex(), static_cast<int>(rows_.size()), static_cast<int>(rows_.size()));

    rows_.push_back(cell::config::DiscType{.name = "Type" + std::to_string(rows_.size()), .radius = 1, .mass = 1});
    discTypeColorMap_[rows_.back().name] = sf::Color::Blue;

    endInsertRows();
}

void DiscTypesTableModel::removeRow(int row)
{
    if (row < 0 || row >= rows_.size())
        return;

    beginRemoveRows(QModelIndex(), row, row);
    discTypeColorMap_.erase(rows_[row].name);
    rows_.erase(rows_.begin() + row);
    endRemoveRows();

    const auto& originalDiscTypes = abstractSimulationBuilder_->getSimulationConfig().discTypes;
    const auto originalIndex = row + removedDiscTypes_.size();

    if (originalIndex < originalDiscTypes.size())
        removedDiscTypes_.insert(originalDiscTypes[originalIndex].name);
}

void DiscTypesTableModel::clearRows()
{
    if (rows_.empty())
        return;

    beginRemoveRows(QModelIndex(), 0, static_cast<int>(rows_.size()) - 1);
    rows_.clear();
    discTypeColorMap_.clear();
    endRemoveRows();

    for (const auto& discType : abstractSimulationBuilder_->getSimulationConfig().discTypes)
        removedDiscTypes_.insert(discType.name);
}

void DiscTypesTableModel::commitChanges()
{
    abstractSimulationBuilder_->setDiscTypes(rows_, removedDiscTypes_, discTypeColorMap_);

    removedDiscTypes_.clear();
}

void DiscTypesTableModel::reload()
{
    removedDiscTypes_.clear();
    beginResetModel();
    rows_ = abstractSimulationBuilder_->getSimulationConfig().discTypes;
    discTypeColorMap_ = abstractSimulationBuilder_->getDiscTypeColorMap();
    endResetModel();
}

void DiscTypesTableModel::updateDiscTypeName(const std::string& newName, int row)
{
    auto& discType = rows_[row];
    auto color = discTypeColorMap_.at(discType.name);
    discTypeColorMap_.erase(discType.name);

    discType.name = newName;
    discTypeColorMap_[newName] = color;
}
