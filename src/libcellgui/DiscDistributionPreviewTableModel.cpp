#include "DiscDistributionPreviewTableModel.hpp"
#include "ColorMapping.hpp"
#include "GlobalSettings.hpp"

DiscDistributionPreviewTableModel::DiscDistributionPreviewTableModel(QObject* parent)
    : QAbstractTableModel(parent)
{
    loadSettings();
}

int DiscDistributionPreviewTableModel::rowCount(const QModelIndex&) const
{
    return distribution_.size();
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

    const auto& [discType, frequency] = distribution_[index.row()];
    switch (index.column())
    {
    case 0:
        return QString::fromStdString(discType.getName());
    case 1:
        return getColorNameMapping()[discType.getColor()];
    case 2:
        return frequency;
    default:
        return {};
    }
}

Qt::ItemFlags DiscDistributionPreviewTableModel::flags(const QModelIndex&) const
{
    return Qt::ItemIsSelectable;
}

void DiscDistributionPreviewTableModel::loadSettings()
{
    distribution_.clear();
    for (const auto& pair : GlobalSettings::getSettings().discTypeDistribution_)
        distribution_.push_back(pair);
}
