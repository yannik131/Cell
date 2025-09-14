#include "models/ReactionsTableModel.hpp"
#include "cell/ExceptionWithLocation.hpp"
#include "core/Utility.hpp"

#include <optional>
#include <unordered_set>

ReactionsTableModel::ReactionsTableModel(QObject* parent, AbstractSimulationBuilder* abstractSimulationBuilder)
    : QAbstractTableModel(parent)
    , abstractSimulationBuilder_(abstractSimulationBuilder)
{
}

int ReactionsTableModel::rowCount(const QModelIndex&) const
{
    return static_cast<int>(rows_.size());
}

int ReactionsTableModel::columnCount(const QModelIndex&) const
{
    return 9;
}

QVariant ReactionsTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal || section > columnCount() - 1)
        return {};

    static const QStringList Headers{"A", "+", "B", "->", "C", "+", "D", "Probability", "Delete"};

    return Headers[section];
}

QVariant ReactionsTableModel::data(const QModelIndex& index, int role) const
{
    if (index.row() >= static_cast<int>(rows_.size()) || (role != Qt::DisplayRole && role != Qt::EditRole))
        return {};

    const auto& reaction = rows_.at(index.row());

    switch (index.column())
    {
    case 0:
        return QString::fromStdString(reaction.educt1);
    case 2:
        return QString::fromStdString(reaction.educt2);
    case 4:
        return QString::fromStdString(reaction.product1);
    case 6:
        return QString::fromStdString(reaction.product2);
    case 7:
        return reaction.probability;
    case 8:
        return "Delete";
    default:
        return {};
    }
}

bool ReactionsTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (index.row() >= static_cast<int>(rows_.size()) || role != Qt::EditRole)
        return false;

    auto& reaction = rows_[index.row()];

    if (index.column() == 0 || index.column() == 2 || index.column() == 4 || index.column() == 6)
    {
        std::string valueString = value.toString().toStdString();
        if (index.column() == 0)
            reaction.educt1 = valueString;
        else if (index.column() == 2)
            reaction.educt2 = valueString;
        else if (index.column() == 4)
            reaction.product1 = valueString;
        else if (index.column() == 6)
            reaction.product2 = valueString;
    }
    else if (index.column() == 7)
        reaction.probability = value.toDouble();
    else
        return false;

    emit dataChanged(index, index);

    return true;
}

Qt::ItemFlags ReactionsTableModel::flags(const QModelIndex& index) const
{
    if (index.row() >= rowCount() || index.column() >= columnCount())
        return {};

    const auto defaultFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    // Reminder: "A", "+", "B", "->", "C", "+", "D", "Probability", "Delete"
    static const QVector<bool> decompositionFlags{true, false, false, false, true, false, true, true, true};
    static const QVector<bool> combinationFlags{true, false, true, false, true, false, false, true, true};
    static const QVector<bool> exchangeFlags{true, false, true, false, true, false, true, true, true};
    static const QVector<bool> transformationFlags{true, false, false, false, true, false, false, true, true};

    switch (types_.at(index.row()))
    {
    case cell::Reaction::Type::Transformation:
        if (!transformationFlags[index.column()])
            return defaultFlags;
        break;
    case cell::Reaction::Type::Combination:
        if (!combinationFlags[index.column()])
            return defaultFlags;
        break;
    case cell::Reaction::Type::Decomposition:
        if (!decompositionFlags[index.column()])
            return defaultFlags;
        break;
    case cell::Reaction::Type::Exchange:
        if (!exchangeFlags[index.column()])
            return defaultFlags;
        break;
    case cell::Reaction::Type::None:
        throw ExceptionWithLocation("Invalid reaction type");
    }

    return defaultFlags | Qt::ItemIsEditable;
}

void ReactionsTableModel::addRow(cell::Reaction::Type type)
{
    if (abstractSimulationBuilder_->getSimulationConfig().discTypes.empty())
        throw ExceptionWithLocation(
            "You're trying to add a reaction without any disc types available. Go define some first.");

    const auto& defaultName = abstractSimulationBuilder_->getSimulationConfig().discTypes.front().name;

    beginInsertRows(QModelIndex(), static_cast<int>(rows_.size()), static_cast<int>(rows_.size()));

    types_.push_back(type);
    cell::config::Reaction newReaction{
        .educt1 = defaultName, .educt2 = defaultName, .product1 = defaultName, .product2 = defaultName};
    switch (type)
    {
    case cell::Reaction::Transformation:
        newReaction.educt2 = newReaction.product2 = "";
        break;
    case cell::Reaction::Decomposition:
        newReaction.educt2 = "";
        break;
    case cell::Reaction::Combination:
        newReaction.product2 = "";
        break;
    default:;
    }
    rows_.push_back(newReaction);

    endInsertRows();
}

void ReactionsTableModel::removeRow(int row)
{
    if (row < 0 || row >= static_cast<int>(rows_.size()))
        return;

    beginRemoveRows(QModelIndex(), row, row);
    rows_.erase(rows_.begin() + row);
    types_.erase(types_.begin() + row);
    endRemoveRows();
}

void ReactionsTableModel::clearRows()
{
    if (rows_.empty())
        return;

    beginRemoveRows(QModelIndex(), 0, static_cast<int>(rows_.size()) - 1);
    rows_.clear();
    types_.clear();
    endRemoveRows();
}

void ReactionsTableModel::commitChanges()
{
    auto config = abstractSimulationBuilder_->getSimulationConfig();
    config.reactions = rows_;

    abstractSimulationBuilder_->setSimulationConfig(config);
}

void ReactionsTableModel::reload()
{
    beginResetModel();
    rows_ = abstractSimulationBuilder_->getSimulationConfig().reactions;
    types_ = inferReactionTypes(rows_);
    endResetModel();
}

std::vector<cell::Reaction::Type>
ReactionsTableModel::inferReactionTypes(const std::vector<cell::config::Reaction>& reactions) const
{
    std::vector<cell::Reaction::Type> types;
    types.reserve(reactions.size());

    for (const auto& reaction : reactions)
        types.push_back(cell::inferType(!reaction.educt2.empty(), !reaction.product2.empty()));

    return types;
}
