#ifndef REACTIONSTABLEMODEL_HPP
#define REACTIONSTABLEMODEL_HPP

#include "Reaction.hpp"

#include <QAbstractTableModel>

/**
 * @brief Model for manipulating the reaction table so the user can add/delete reactions
 */
class ReactionsTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ReactionsTableModel(QObject* parent = nullptr);

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

    /**
     * @brief Adds a row from the given reaction, filling it with the actual values
     */
    void addRowFromReaction(const cell::Reaction& reaction);

    /**
     * @brief Creates a new row with a possibly invalid reaction (educt and product masses may not add up etc.) based on
     * the given type
     */
    void addEmptyRow(const cell::Reaction::Type& type);

    /**
     * @brief Removes the reaction from the model at the given row
     */
    void removeRow(int row);

    /**
     * @brief Resets the model to the reactions that are already in the current settings
     */
    void loadSettings();

    /**
     * @brief Saves the reactions in the model to the settings, throwing and exception if something is incorrect (i. e.
     * educt and product masses don't add up etc.)
     */
    void saveSettings();

    /**
     * @brief Removes all rows from the model
     */
    void clearRows();

signals:
    void reactionsChanged();

private:
    std::vector<cell::Reaction> rows_;
};

#endif /* REACTIONSTABLEMODEL_HPP */
