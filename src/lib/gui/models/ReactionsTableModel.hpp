#ifndef REACTIONSTABLEMODEL_HPP
#define REACTIONSTABLEMODEL_HPP

#include "cell/Reaction.hpp"
#include "cell/SimulationConfig.hpp"
#include "core/AbstractSimulationBuilder.hpp"

#include <QAbstractTableModel>

/**
 * @brief Model for manipulating the reaction table so the user can add/delete reactions
 */
class ReactionsTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ReactionsTableModel(QObject* parent, AbstractSimulationBuilder* abstractSimulationBuilder);
    /**
     * @returns number of reactions currently in the model
     */
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    /**
     * @returns 9: "A", "+", "B", "->", "C", "+", "D", "Probability", "Delete"
     */
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    /**
     * @returns the headers for the columns, see `columnCount`
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    /**
     * @brief Extracts the products/educts from the reaction of the row and returns that data
     */
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    /**
     * @brief Sets educts/products of the reaction in the given row based on the table content
     */
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;

    /**
     * @brief Except for exchange reactions, the other 3 reaction types may not have educt2 or product2, so the flags
     * need to make these columns uneditable
     */
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    void addRow(cell::Reaction::Type type);

    /**
     * @brief Removes the reaction from the model at the given row
     */
    void removeRow(int row);

    /**
     * @brief Removes all rows from the model
     */
    void clearRows();

    void commitChanges();
    void discardChanges();

private:
    std::vector<cell::Reaction::Type> inferReactionTypes(const std::vector<cell::config::Reaction>& reactions) const;

private:
    std::vector<cell::config::Reaction> rows_;
    std::vector<cell::Reaction::Type> types_;

    AbstractSimulationBuilder* abstractSimulationBuilder_ = nullptr;
};

#endif /* REACTIONSTABLEMODEL_HPP */
