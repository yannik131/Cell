#include "models/DiscTableModel.hpp"
#include "core/AbstractSimulationBuilder.hpp"
#include "models/DiscTableModel.hpp"

DiscTableModel::DiscTableModel(QObject* parent, AbstractSimulationBuilder* abstractSimulationBuilder)
    : QAbstractTableModel(parent)
    , abstractSimulationBuilder_(abstractSimulationBuilder)
{
    abstractSimulationBuilder_->registerConfigObserver(
        [&](const cell::SimulationConfig& config)
        {
            beginResetModel();
            rows_ = config.setup.discs;
            endResetModel();
        });
}

int DiscTableModel::rowCount(const QModelIndex& parent) const
{
    return rows_.size();
}

int DiscTableModel::columnCount(const QModelIndex& parent) const
{
    return 6;
}

QVariant DiscTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal || section > columnCount() - 1)
        return {};

    static const QStringList Headers{"Disc type", "x", "y", "v_x", "v_y", "Delete"};

    return Headers[section];
}

QVariant DiscTableModel::data(const QModelIndex& index, int role) const
{
    if (index.row() >= static_cast<int>(rows_.size()) || (role != Qt::DisplayRole && role != Qt::EditRole))
        return {};

    const auto& disc = rows_.at(index.row());

    switch (index.column())
    {
    case 0:
        return QString::fromStdString(disc.discTypeName);
    case 1:
        return disc.x;
    case 2:
        return disc.y;
    case 3:
        return disc.vx;
    case 4:
        return disc.vy;
    case 5:
        return "Delete";
    default:
        return {};
    }
}

bool DiscTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (index.row() >= static_cast<int>(rows_.size()) || role != Qt::EditRole)
        return false;

    auto& disc = rows_[index.row()];

    switch (index.column())
    {
    case 0:
        disc.discTypeName = value.toString().toStdString();
        break;
    case 1:
        disc.x = value.toDouble();
        break;
    case 2:
        disc.y = value.toDouble();
        break;
    case 3:
        disc.vx = value.toDouble();
        break;
    case 4:
        disc.vy = value.toDouble();
        break;
    default:
        return false;
    }

    emit dataChanged(index, index);

    return true;
}

Qt::ItemFlags DiscTableModel::flags(const QModelIndex& index) const
{
    const auto& defaultFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    if (index.column() == 5)
        return defaultFlags;

    return defaultFlags | Qt::ItemIsEditable;
}

void DiscTableModel::removeRow(int row)
{
    if (row < 0 || row >= static_cast<int>(rows_.size()))
        return;

    beginRemoveRows(QModelIndex(), row, row);
    rows_.erase(rows_.begin() + row);
    endRemoveRows();
}

void DiscTableModel::addRow()
{
    const auto& discTypes = abstractSimulationBuilder_->getSimulationConfig().discTypes;
    if (discTypes.empty())
        throw ExceptionWithLocation("No disc types available. Define some first.");

    beginInsertRows(QModelIndex(), static_cast<int>(rows_.size()), static_cast<int>(rows_.size()));
    rows_.push_back(cell::config::Disc{.discTypeName = discTypes.front().name, .x = 0, .y = 0, .vx = 0, .vy = 0});
    endInsertRows();
}

void DiscTableModel::clearRows()
{
    if (rows_.empty())
        return;

    beginRemoveRows(QModelIndex(), 0, static_cast<int>(rows_.size()) - 1);
    rows_.clear();
    endRemoveRows();
}

const std::vector<cell::config::Disc>& DiscTableModel::getRows() const
{
    return rows_;
}
