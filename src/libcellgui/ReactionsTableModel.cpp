#include "ReactionsTableModel.hpp"
#include "GlobalSettings.hpp"

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
        return QString::fromStdString(reaction.educt1_.name_);
    case 2:
        return QString::fromStdString(reaction.educt2_.name_);
    case 4:
        return QString::fromStdString(reaction.product1_.name_);
    case 6:
        return QString::fromStdString(reaction.product2_.name_);
    case 7:
        return reaction.probability_;
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

    auto& reaction = rows_[index.row()];
    switch (index.column())
    {
    case 0:
        reaction.educt1_ = GlobalSettings::getDiscTypeByName(value.toString().toStdString());
        break;
    case 2:
        reaction.educt2_ = GlobalSettings::getDiscTypeByName(value.toString().toStdString());
        break;
    case 4:
        reaction.product1_ = GlobalSettings::getDiscTypeByName(value.toString().toStdString());
        break;
    case 6:
        reaction.product2_ = GlobalSettings::getDiscTypeByName(value.toString().toStdString());
        break;
    case 7:
        reaction.probability_ = value.toFloat();
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

    auto defaultFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    // Reminder: "A", "+", "B", "->", "C", "+", "D", "Probability [%]", "Delete"
    static const QVector<bool> decompositionFlags{true, false, false, false, true, false, true, true, true};
    static const QVector<bool> combinationFlags{true, false, true, false, true, false, false, true, true};
    static const QVector<bool> exchangeFlags{true, false, true, false, true, false, true, true, true};

    const auto& reaction = rows_.at(index.row());
    const auto& reactionType = inferReactionType switch (inferReactionType(reaction))
    {
    case Decomposition:
        if (decompositionFlags[index.column()])
            defaultFlags |=
    }
}

void ReactionsTableModel::addRowFromReaction(const Reaction& reaction)
{
}

void ReactionsTableModel::removeRow(int row)
{
}

void ReactionsTableModel::loadSettings()
{
}

void ReactionsTableModel::saveSettings()
{
}

void ReactionsTableModel::clearRows()
{
}
