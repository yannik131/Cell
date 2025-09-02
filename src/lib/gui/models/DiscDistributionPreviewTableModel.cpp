#include "models/DiscDistributionPreviewTableModel.hpp"
#include "core/ColorMapping.hpp"

DiscDistributionPreviewTableModel::DiscDistributionPreviewTableModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

int DiscDistributionPreviewTableModel::rowCount(const QModelIndex&) const
{
    return 0;
}

int DiscDistributionPreviewTableModel::columnCount(const QModelIndex&) const
{
    return 3;
}

QVariant DiscDistributionPreviewTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal || section >= columnCount())
        return {};

    static const QStringList Headers{"Type", "Color", "%"};

    return Headers[section];
}

QVariant DiscDistributionPreviewTableModel::data(const QModelIndex& index, int role) const
{
    if (index.row() >= rowCount() || role != Qt::DisplayRole)
        return {};
}

Qt::ItemFlags DiscDistributionPreviewTableModel::flags(const QModelIndex&) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}
