#include "models/ReactionsTableModel.hpp"
#include "cell/ExceptionWithLocation.hpp"
#include "core/Utility.hpp"

#include <optional>
#include <unordered_set>

ReactionsTableModel::ReactionsTableModel(QObject* parent)
    : QAbstractTableModel(parent)
{
    loadSettings();
}

int ReactionsTableModel::rowCount(const QModelIndex&) const
{
    return 0;
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
    return {};
}

bool ReactionsTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    return false;
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

    return defaultFlags | Qt::ItemIsEditable;
}

void ReactionsTableModel::removeRow(int row)
{
    return;
}

void ReactionsTableModel::loadSettings()
{
    return;
}

void ReactionsTableModel::saveSettings()
{
    emit reactionsChanged();
}

void ReactionsTableModel::clearRows()
{
    return;
}
