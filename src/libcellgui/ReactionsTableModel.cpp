#include "ReactionsTableModel.hpp"
#include "GlobalSettings.hpp"

#include <unordered_set>

ReactionsTableModel::ReactionsTableModel(QObject* parent)
{
}

int ReactionsTableModel::rowCount(const QModelIndex& parent) const
{
    return rows_.size();
}

int ReactionsTableModel::columnCount(const QModelIndex& parent) const
{
    // Reminder: "A", "+", "B", "->", "C", "+", "D", "Probability [%]", "Delete"
    return 9;
}

QVariant ReactionsTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal || section > columnCount() - 1)
        return {};

    static const QStringList Headers{"A", "+", "B", "->", "C", "+", "D", "Probability [%]", "Delete"};

    return Headers[section];
}

QVariant ReactionsTableModel::data(const QModelIndex& index, int role) const
{
    if (index.row() >= rows_.size() || (role != Qt::DisplayRole && role != Qt::EditRole))
        return {};

    const auto& reaction = rows_.at(index.row());

    switch (index.column())
    {
    case 0:
        return QString::fromStdString(reaction.getEduct1().getName());
    case 2:
        if (reaction.hasEduct2())
            return QString::fromStdString(reaction.getEduct2().getName());
        return {};
    case 4:
        return QString::fromStdString(reaction.getProduct1().getName());
    case 6:
        if (reaction.hasProduct2())
            return QString::fromStdString(reaction.getProduct2().getName());
        return {};
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
    if (index.row() >= rows_.size() || role != Qt::EditRole)
        return false;

    const auto& discType = GlobalSettings::getDiscTypeByName(value.toString().toStdString());
    auto& reaction = rows_[index.row()];
    switch (index.column())
    {
    case 0:
        reaction.setEduct1(discType);
        break;
    case 2:
        reaction.setEduct2(discType);
        break;
    case 4:
        reaction.setProduct1(discType);
        break;
    case 6:
        reaction.setProduct2(discType);
        break;
    case 7:
        reaction.setProbability(value.toFloat());
        break;
    default:
        return false;
    }

    if (index.column() < columnCount())
        emit dataChanged(index, index);

    return true;
}

Qt::ItemFlags ReactionsTableModel::flags(const QModelIndex& index) const
{
    if (index.row() >= rowCount() || index.column() >= columnCount())
        return {};

    const auto defaultFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    // Reminder: "A", "+", "B", "->", "C", "+", "D", "Probability [%]", "Delete"
    static const QVector<bool> decompositionFlags{true, false, false, false, true, false, true, true, true};
    static const QVector<bool> combinationFlags{true, false, true, false, true, false, false, true, true};
    static const QVector<bool> exchangeFlags{true, false, true, false, true, false, true, true, true};

    const auto& reaction = rows_.at(index.row());
    switch (reaction.getType())
    {
    case Reaction::Type::Combination:
        if (!combinationFlags[index.column()])
            return defaultFlags;
        break;
    case Reaction::Type::Decomposition:
        if (!decompositionFlags[index.column()])
            return defaultFlags;
        break;
    case Reaction::Type::Exchange:
        if (!exchangeFlags[index.column()])
            return defaultFlags;
        break;
    }

    return defaultFlags | Qt::ItemIsEditable;
}

void ReactionsTableModel::addRowFromReaction(const Reaction& reaction)
{
    beginInsertRows(QModelIndex(), rows_.size(), rows_.size());
    rows_.push_back(reaction);
    endInsertRows();
}

void ReactionsTableModel::removeRow(int row)
{
    if (row < 0 || row >= rows_.size())
        return;

    beginRemoveRows(QModelIndex(), row, row);
    rows_.erase(rows_.begin() + row);
    endRemoveRows();
}

void ReactionsTableModel::loadSettings()
{
    clearRows();

    beginInsertRows(QModelIndex(), 0, 0);

    const auto& settings = GlobalSettings::getSettings();

    // Reactions for {A, B} are duplicated in the maps with {B, A} for easier lookup
    // Gotta ignore those duplicates here
    std::unordered_set<Reaction, ReactionHash> reactionSet;

    auto collectReactions = [&](const auto& reactionsMap)
    {
        for (const auto& [key, reactions] : reactionsMap)
        {
            for (const auto& reaction : reactions)
                reactionSet.insert(reaction);
        }
    };

    collectReactions(settings.decompositionReactions_);
    collectReactions(settings.combinationReactions_);
    collectReactions(settings.exchangeReactions_);

    for (const auto& reaction : reactionSet)
        rows_.push_back(reaction);

    endInsertRows();
}

void ReactionsTableModel::saveSettings()
{
    GlobalSettings::get().clearReactions();
    for (const auto& reaction : rows_)
        GlobalSettings::get().addReaction(reaction);

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
