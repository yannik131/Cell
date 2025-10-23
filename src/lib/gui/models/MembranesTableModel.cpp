#include "models/MembranesTableModel.hpp"
#include "core/AbstractSimulationBuilder.hpp"

MembranesTableModel::MembranesTableModel(QObject* parent, AbstractSimulationBuilder* abstractSimulationBuilder)
    : QAbstractTableModel(parent)
    , abstractSimulationBuilder_(abstractSimulationBuilder)
{
}

int MembranesTableModel::rowCount(const QModelIndex& parent) const
{
    return static_cast<int>(rows_.size());
}

int MembranesTableModel::columnCount(const QModelIndex& parent) const
{
    return 4;
}

QVariant MembranesTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal || section > columnCount() - 1)
        return {};

    static const QStringList Headers{"Membrane type", "x", "y", "Delete"};

    return Headers[section];
}

QVariant MembranesTableModel::data(const QModelIndex& index, int role) const
{
    if (index.row() >= static_cast<int>(rows_.size()) || (role != Qt::DisplayRole && role != Qt::EditRole))
        return {};

    const auto& membrane = rows_.at(index.row());

    switch (index.column())
    {
    case 0: return QString::fromStdString(membrane.membraneTypeName);
    case 1: return membrane.x;
    case 2: return membrane.y;
    case 3: return "Delete";
    default: return {};
    }
}

bool MembranesTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (index.row() >= static_cast<int>(rows_.size()) || role != Qt::EditRole)
        return false;

    auto& membrane = rows_[index.row()];

    switch (index.column())
    {
    case 0: membrane.membraneTypeName = value.toString().toStdString(); break;
    case 1: membrane.x = value.toDouble(); break;
    case 2: membrane.y = value.toDouble(); break;
    default: return false;
    }

    emit dataChanged(index, index);

    return true;
}

Qt::ItemFlags MembranesTableModel::flags(const QModelIndex& index) const
{
    const auto& defaultFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    if (index.column() == 3)
        return defaultFlags;

    return defaultFlags | Qt::ItemIsEditable;
}

void MembranesTableModel::removeRow(int row)
{
    if (row < 0 || row >= static_cast<int>(rows_.size()))
        return;

    beginRemoveRows(QModelIndex(), row, row);
    rows_.erase(rows_.begin() + row);
    endRemoveRows();
}

void MembranesTableModel::addRow()
{
    const auto& membraneTypes = abstractSimulationBuilder_->getSimulationConfig().membraneTypes;
    if (membraneTypes.empty())
        throw ExceptionWithLocation("No membrane types available. Define some first.");

    beginInsertRows(QModelIndex(), static_cast<int>(rows_.size()), static_cast<int>(rows_.size()));
    rows_.push_back(cell::config::Membrane{.membraneTypeName = membraneTypes.front().name, .x = 0, .y = 0});
    endInsertRows();
}

void MembranesTableModel::clearRows()
{
    beginResetModel();
    rows_.clear();
    endResetModel();
}

const std::vector<cell::config::Membrane>& MembranesTableModel::getRows() const
{
    return rows_;
}

void MembranesTableModel::reload()
{
    beginResetModel();
    rows_ = abstractSimulationBuilder_->getSimulationConfig().setup.membranes;
    endResetModel();
}
