#include "models/ReactionsTableModel.hpp"
#include "cell/ExceptionWithLocation.hpp"
#include "core/SimulationConfigUpdater.hpp"
#include "core/Utility.hpp"

#include "ReactionsTableModel.hpp"
#include <optional>
#include <unordered_set>

ReactionsTableModel::ReactionsTableModel(QObject* parent, SimulationConfigUpdater* simulationConfigUpdater)
    : AbstractSimulationConfigTableModel<cell::config::Reaction>(
          parent, {{"A", "+", "B", "->", "C", "+", "D", "Probability", "Delete"}}, simulationConfigUpdater)
{
}

void ReactionsTableModel::addRow(cell::Reaction::Type type)
{
    if (simulationConfigUpdater_->getSimulationConfig().discTypes.empty())
        throw ExceptionWithLocation(
            "You're trying to add a reaction without any disc types available. Go define some first.");

    const auto& defaultName = simulationConfigUpdater_->getSimulationConfig().discTypes.front().name;

    beginInsertRows(QModelIndex(), static_cast<int>(rows_.size()), static_cast<int>(rows_.size()));

    cell::config::Reaction newReaction{
        .educt1 = defaultName, .educt2 = defaultName, .product1 = defaultName, .product2 = defaultName};
    switch (type)
    {
    case cell::Reaction::Type::Transformation: newReaction.educt2 = newReaction.product2 = ""; break;
    case cell::Reaction::Type::Decomposition: newReaction.educt2 = ""; break;
    case cell::Reaction::Type::Combination: newReaction.product2 = ""; break;
    default:;
    }
    rows_.push_back(newReaction);

    endInsertRows();
}

void ReactionsTableModel::loadFromConfig()
{
    setRows(simulationConfigUpdater_->getSimulationConfig().reactions);
}

void ReactionsTableModel::saveToConfig()
{
    auto config = simulationConfigUpdater_->getSimulationConfig();
    config.reactions = rows_;

    simulationConfigUpdater_->setSimulationConfig(config);
}

QVariant ReactionsTableModel::getField(const cell::config::Reaction& row, const QModelIndex& index) const
{
    switch (index.column())
    {
    case 0: return QString::fromStdString(row.educt1);
    case 2: return QString::fromStdString(row.educt2);
    case 4: return QString::fromStdString(row.product1);
    case 6: return QString::fromStdString(row.product2);
    case 7: return row.probability;
    case 8: return "Delete";
    default: return {};
    }
}

bool ReactionsTableModel::setField(cell::config::Reaction& row, const QModelIndex& index, const QVariant& value)
{
    int column = index.column();
    if (column == 0 || column == 2 || column == 4 || column == 6)
    {
        std::string valueString = value.toString().toStdString();
        if (column == 0)
            row.educt1 = valueString;
        else if (column == 2)
            row.educt2 = valueString;
        else if (column == 4)
            row.product1 = valueString;
        else if (column == 6)
            row.product2 = valueString;
    }
    else if (column == 7)
        row.probability = value.toDouble();
    else
        return false;

    return true;
}

bool ReactionsTableModel::isEditable(const QModelIndex& index) const
{
    // Reminder: "A", "+", "B", "->", "C", "+", "D", "Probability", "Delete"
    static const QVector<bool> decompositionFlags{true, false, false, false, true, false, true, true, true};
    static const QVector<bool> combinationFlags{true, false, true, false, true, false, false, true, true};
    static const QVector<bool> exchangeFlags{true, false, true, false, true, false, true, true, true};
    static const QVector<bool> transformationFlags{true, false, false, false, true, false, false, true, true};

    const auto& reaction = rows_[index.row()];
    const auto column = index.column();

    switch (cell::inferReactionType(!reaction.educt2.empty(), !reaction.product2.empty()))
    {
    case cell::Reaction::Type::Transformation: return transformationFlags[column]; break;
    case cell::Reaction::Type::Combination: return combinationFlags[column];
    case cell::Reaction::Type::Decomposition: return decompositionFlags[column];
    case cell::Reaction::Type::Exchange: return exchangeFlags[column];
    case cell::Reaction::Type::None:
    default: throw ExceptionWithLocation("Invalid reaction type");
    }
}
