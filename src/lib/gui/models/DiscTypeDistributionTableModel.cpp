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
    return 0;
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
    return {};
}

bool DiscTypeDistributionTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    return false;
}

Qt::ItemFlags DiscTypeDistributionTableModel::flags(const QModelIndex&) const
{
    return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void DiscTypeDistributionTableModel::addRowFromDiscType(const cell::DiscType& discType)
{
}

void DiscTypeDistributionTableModel::addEmptyRow()
{
}

void DiscTypeDistributionTableModel::removeRow(int row)
{
}

void DiscTypeDistributionTableModel::loadSettings()
{
}

void DiscTypeDistributionTableModel::saveSettings()
{
}

void DiscTypeDistributionTableModel::clearRows()
{
}
