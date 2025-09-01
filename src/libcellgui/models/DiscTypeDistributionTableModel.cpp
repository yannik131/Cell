#include "DiscTypeDistributionTableModel.hpp"
#include "ColorMapping.hpp"

#include <set>

DiscTypeDistributionTableModel::DiscTypeDistributionTableModel(QObject* parent)
    : QAbstractTableModel(parent)
{
    loadSettings();
}

int DiscTypeDistributionTableModel::rowCount(const QModelIndex&) const
{
    return static_cast<int>(rows_.size());
}

int DiscTypeDistributionTableModel::columnCount(const QModelIndex&) const
{
    return 6;
}

QVariant DiscTypeDistributionTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal || section > columnCount() - 1)
        return {};

    static const QStringList Headers{"Disc Type Name", "Radius", "Mass", "Color", "Frequency", "Delete"};

    return Headers[section];
}

QVariant DiscTypeDistributionTableModel::data(const QModelIndex& index, int role) const
{
    if (index.row() >= rows_.size() || (role != Qt::DisplayRole && role != Qt::EditRole))
        return {};

    const auto& [discType, frequency] = rows_.at(index.row());

    switch (index.column())
    {
    case 0:
        return QString::fromStdString(discType.getName());
    case 1:
        return discType.getRadius();
    case 2:
        return discType.getMass();
    case 3:
        return "Green";
    case 4:
        return frequency;
    case 5:
        return "Delete";
    default:
        return {};
    }
}

bool DiscTypeDistributionTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (index.row() >= rows_.size() || role != Qt::EditRole)
        return false;

    auto& [discType, frequency] = rows_[index.row()];
    switch (index.column())
    {
    case 0:
        discType.setName(value.toString().toStdString());
        break;
    case 1:
        discType.setRadius(value.toFloat());
        break;
    case 2:
        discType.setMass(value.toFloat());
        break;
    case 3:
        break;
    case 4:
        frequency = value.toInt();
        break;
    default:
        return false;
    }

    if (index.column() < 5)
        emit dataChanged(index, index);

    return true;
}

Qt::ItemFlags DiscTypeDistributionTableModel::flags(const QModelIndex&) const
{
    return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void DiscTypeDistributionTableModel::addRowFromDiscType(const cell::DiscType& discType)
{
    beginInsertRows(QModelIndex(), static_cast<int>(rows_.size()), static_cast<int>(rows_.size()));
    endInsertRows();
}

void DiscTypeDistributionTableModel::addEmptyRow()
{
    beginInsertRows(QModelIndex(), static_cast<int>(rows_.size()), static_cast<int>(rows_.size()));
    endInsertRows();
}

void DiscTypeDistributionTableModel::removeRow(int row)
{
    if (row < 0 || row >= rows_.size())
        return;

    beginRemoveRows(QModelIndex(), row, row);
    rows_.erase(rows_.begin() + row);
    endRemoveRows();
}

void DiscTypeDistributionTableModel::loadSettings()
{
    clearRows();

    endInsertRows();
}

void DiscTypeDistributionTableModel::saveSettings()
{
}

void DiscTypeDistributionTableModel::clearRows()
{
    if (rows_.empty())
        return;

    beginRemoveRows(QModelIndex(), 0, static_cast<int>(rows_.size()) - 1);
    rows_.clear();
    endRemoveRows();
}
