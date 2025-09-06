#include "models/DiscTypeDistributionTableModel.hpp"

DiscTypeDistributionTableModel::DiscTypeDistributionTableModel(QObject* parent)
{
}

int DiscTypeDistributionTableModel::rowCount(const QModelIndex& parent) const
{
    return 0;
}

int DiscTypeDistributionTableModel::columnCount(const QModelIndex& parent) const
{
    return 0;
}

QVariant DiscTypeDistributionTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QVariant();
}

QVariant DiscTypeDistributionTableModel::data(const QModelIndex& index, int role) const
{
    return QVariant();
}

bool DiscTypeDistributionTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    return false;
}

Qt::ItemFlags DiscTypeDistributionTableModel::flags(const QModelIndex& index) const
{
    return Qt::ItemFlags();
}
