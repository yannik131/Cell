#include "models/DiscTableModel.hpp"

DiscTableModel::DiscTableModel(QObject* parent)
{
}

int DiscTableModel::rowCount(const QModelIndex& parent) const
{
    return 0;
}

int DiscTableModel::columnCount(const QModelIndex& parent) const
{
    return 0;
}

QVariant DiscTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QVariant();
}

QVariant DiscTableModel::data(const QModelIndex& index, int role) const
{
    return QVariant();
}

bool DiscTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    return false;
}

Qt::ItemFlags DiscTableModel::flags(const QModelIndex& index) const
{
    return Qt::ItemFlags();
}
