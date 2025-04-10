#include "DiscTypeDistributionTableModel.hpp"
#include "ColorMapping.hpp"
#include "GlobalSettings.hpp"

DiscTypeDistributionTableModel::DiscTypeDistributionTableModel(QObject* parent)
    : QAbstractTableModel(parent)
{
    loadSettings();
}

int DiscTypeDistributionTableModel::rowCount(const QModelIndex& parent) const
{
    return rows_.size();
}

int DiscTypeDistributionTableModel::columnCount(const QModelIndex& parent) const
{
    return 6;
}

QVariant DiscTypeDistributionTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
    {
        switch (section)
        {
        case 0: return "Disc Type Name";
        case 1: return "Radius";
        case 2: return "Mass";
        case 3: return "Color";
        case 4: return "Frequency";
        case 5: return "Delete";
        default: return QVariant();
        }
    }

    return QVariant();
}

QVariant DiscTypeDistributionTableModel::data(const QModelIndex& index, int role) const
{
    if (index.row() >= rows_.size() || (role != Qt::DisplayRole && role != Qt::EditRole))
        return {};

    const auto& [discType, frequency] = rows_.at(index.row());

    switch (index.column())
    {
    case 0: return QString::fromStdString(discType.name_);
    case 1: return discType.radius_;
    case 2: return discType.mass_;
    case 3: return ColorNameMapping[discType.color_];
    case 4: return frequency;
    case 5: return "Delete";
    default: return {};
    }
}

bool DiscTypeDistributionTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (index.row() >= rows_.size() || role != Qt::EditRole)
        return false;

    auto& [discType, frequency] = rows_[index.row()];
    switch (index.column())
    {
    case 0: discType.name_ = value.toString().toStdString(); break;
    case 1: discType.radius_ = value.toInt(); break;
    case 2: discType.mass_ = value.toInt(); break;
    case 3: discType.color_ = NameColorMapping[value.toString()]; break;
    case 4: frequency = value.toInt(); break;
    default: return false;
    }

    if (index.column() < 5)
        emit dataChanged(index, index);

    return true;
}

Qt::ItemFlags DiscTypeDistributionTableModel::flags(const QModelIndex& index) const
{
    return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void DiscTypeDistributionTableModel::addRowFromDiscType(const DiscType& discType)
{
    beginInsertRows(QModelIndex(), rows_.size(), rows_.size());
    rows_.push_back({discType, 0});
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

    beginInsertRows(QModelIndex(), 0, 0);
    for (const auto& pair : GlobalSettings::getSettings().discTypeDistribution_)
        rows_.push_back(pair);
    endInsertRows();
}

void DiscTypeDistributionTableModel::saveSettings()
{
    std::map<DiscType, int> result(rows_.begin(), rows_.end());
    GlobalSettings::get().setDiscTypeDistribution(std::move(result));
}

void DiscTypeDistributionTableModel::clearRows()
{
    if (rows_.empty())
        return;

    beginRemoveRows(QModelIndex(), 0, static_cast<int>(rows_.size()) - 1);
    rows_.clear();
    endRemoveRows();
}
