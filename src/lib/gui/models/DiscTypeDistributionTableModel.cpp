#include "models/DiscTypeDistributionTableModel.hpp"
#include "core/AbstractSimulationBuilder.hpp"

#include "DiscTypeDistributionTableModel.hpp"
#include <string>
#include <unordered_set>

DiscTypeDistributionTableModel::DiscTypeDistributionTableModel(QObject* parent,
                                                               AbstractSimulationBuilder* abstractSimulationBuilder)
    : QAbstractTableModel(parent)
    , abstractSimulationBuilder_(abstractSimulationBuilder)
{
    abstractSimulationBuilder_->registerConfigObserver(
        [&](const cell::SimulationConfig& config, const std::map<std::string, sf::Color>&)
        {
            beginResetModel();
            rows_.assign(config.setup.distribution.begin(), config.setup.distribution.end());
            endResetModel();
        });
}

int DiscTypeDistributionTableModel::rowCount(const QModelIndex& parent) const
{
    return static_cast<int>(rows_.size());
}

int DiscTypeDistributionTableModel::columnCount(const QModelIndex& parent) const
{
    return 3;
}

QVariant DiscTypeDistributionTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal || section > columnCount() - 1)
        return {};

    static const QStringList Headers{"Disc type", "Frequency", "Delete"};

    return Headers[section];
}

QVariant DiscTypeDistributionTableModel::data(const QModelIndex& index, int role) const
{
    if (index.row() >= static_cast<int>(rows_.size()) || (role != Qt::DisplayRole && role != Qt::EditRole))
        return {};

    const auto& [discType, frequency] = rows_.at(index.row());

    switch (index.column())
    {
    case 0:
        return QString::fromStdString(discType);
    case 1:
        return frequency;
    case 2:
        return "Delete";
    default:
        return {};
    }
}

bool DiscTypeDistributionTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (index.row() >= static_cast<int>(rows_.size()) || role != Qt::EditRole)
        return false;

    auto& [discType, frequency] = rows_[index.row()];

    if (index.column() == 0)
        discType = value.toString().toStdString();
    else if (index.column() == 1)
        frequency = value.toDouble();
    else
        return false;

    emit dataChanged(index, index);

    return true;
}

Qt::ItemFlags DiscTypeDistributionTableModel::flags(const QModelIndex& index) const
{
    const auto& defaultFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    if (index.column() == 2)
        return defaultFlags;

    return defaultFlags | Qt::ItemIsEditable;
}

void DiscTypeDistributionTableModel::removeRow(int row)
{
    if (row < 0 || row >= static_cast<int>(rows_.size()))
        return;

    beginRemoveRows(QModelIndex(), row, row);
    rows_.erase(rows_.begin() + row);
    endRemoveRows();
}

void DiscTypeDistributionTableModel::addRow()
{
    const auto& discTypes = abstractSimulationBuilder_->getSimulationConfig().discTypes;
    std::unordered_set<std::string> availableNames;

    for (const auto& discType : discTypes)
        availableNames.insert(discType.name);

    for (const auto& [name, frequency] : rows_)
        availableNames.erase(name);

    if (availableNames.empty())
        throw ExceptionWithLocation("There are no disc types available that are not already in the distribution.");

    beginInsertRows(QModelIndex(), static_cast<int>(rows_.size()), static_cast<int>(rows_.size()));
    rows_.push_back(std::make_pair(*availableNames.begin(), 0));
    endInsertRows();
}

void DiscTypeDistributionTableModel::clearRows()
{
    if (rows_.empty())
        return;

    beginRemoveRows(QModelIndex(), 0, static_cast<int>(rows_.size()) - 1);
    rows_.clear();
    endRemoveRows();
}

const std::vector<std::pair<std::string, double>>& DiscTypeDistributionTableModel::getRows() const
{
    return rows_;
}
