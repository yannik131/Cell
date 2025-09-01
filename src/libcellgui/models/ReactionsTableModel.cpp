#include "ReactionsTableModel.hpp"
#include "ExceptionWithLocation.hpp"
#include "Utility.hpp"

#include <optional>
#include <unordered_set>

ReactionsTableModel::ReactionsTableModel(QObject* parent)
    : QAbstractTableModel(parent)
{
    loadSettings();
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
        return "educt1";
    case 2:
        return "educt2";
    case 4:
        return "product1";
    case 6:
        return "product2";
    case 7:
        return reaction.getProbability();
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
        cell::DiscTypeID ID = 0;
        if (index.column() == 0)
            reaction.setEduct1(ID);
        else if (index.column() == 2)
            reaction.setEduct2(ID);
        else if (index.column() == 4)
            reaction.setProduct1(ID);
        else if (index.column() == 6)
            reaction.setProduct2(ID);
    }
    else if (index.column() == 7)
        reaction.setProbability(value.toFloat());
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

    const auto& reaction = rows_.at(index.row());
    switch (reaction.getType())
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

void ReactionsTableModel::addRowFromReaction(const cell::Reaction& reaction)
{
    beginInsertRows(QModelIndex(), static_cast<int>(rows_.size()), static_cast<int>(rows_.size()));
    rows_.push_back(reaction);
    endInsertRows();
}

void ReactionsTableModel::addEmptyRow(const cell::Reaction::Type& type)
{
    switch (type)
    {
    case cell::Reaction::Type::Transformation:
        break;
    case cell::Reaction::Type::Combination:
        break;
    case cell::Reaction::Type::Decomposition:
        break;
    case cell::Reaction::Type::Exchange:
        break;
    case cell::Reaction::Type::None:
        throw ExceptionWithLocation("Invalid reaction type");
    }
}

void ReactionsTableModel::removeRow(int row)
{
    if (row < 0 || row >= static_cast<int>(rows_.size()))
        return;

    beginRemoveRows(QModelIndex(), row, row);
    rows_.erase(rows_.begin() + row);
    endRemoveRows();
}

void ReactionsTableModel::loadSettings()
{
    clearRows();

    // Reactions for {A, B} are duplicated in the maps with {B, A} for easier lookup
    // Gotta ignore those duplicates here
    std::unordered_set<cell::Reaction, cell::ReactionHash> reactionSet;

    auto collectReactions = [&](const auto& reactionsMap)
    {
        for (const auto& [key, reactions] : reactionsMap)
        {
            for (const auto& reaction : reactions)
                reactionSet.insert(reaction);
        }
    };

    if (reactionSet.empty())
        return;

    beginInsertRows(QModelIndex(), 0, static_cast<int>(reactionSet.size()) - 1);

    for (const auto& reaction : reactionSet)
        rows_.push_back(reaction);

    endInsertRows();
}

void ReactionsTableModel::saveSettings()
{
    emit reactionsChanged();
}

void ReactionsTableModel::clearRows()
{
    // TODO duplicate of DiscTypeDistributionTableModel::clearRows
    if (rows_.empty())
        return;

    beginRemoveRows(QModelIndex(), 0, static_cast<int>(rows_.size()) - 1);
    rows_.clear();
    endRemoveRows();
}
