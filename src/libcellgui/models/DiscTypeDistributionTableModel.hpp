#ifndef DISCTYPEDISTRIBUTIONTABLEMODEL_HPP
#define DISCTYPEDISTRIBUTIONTABLEMODEL_HPP

#include "DiscType.hpp"

#include <QAbstractTableModel>
#include <QVector>

/**
 * @brief Model for managing the disc type distribution of the simulation. Used to display the distribution in the
 * DiscTypeDistributionDialog
 */
class DiscTypeDistributionTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit DiscTypeDistributionTableModel(QObject* parent = nullptr);

    /**
     * @returns Number of disc types in the currently edited distribution
     */
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    /**
     * @returns 6: Disc type name, radius, mass, color, frequency, delete button
     */
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    /**
     * @returns the headers for each column in human readable form (see `columnCount`)
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    /**
     * @returns content for a single cell for the given disc type
     */
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    /**
     * @returns `true` if for the disc type in the given row the given column can be set to the appropriate value and
     * the role is `QtEditRole`, `false` otherwise
     */
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;

    /**
     * @brief The disc type distribution table has now columns that can't be edited or selected
     */
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    /**
     * @brief Creates a new row in the table with information of the given disc type, used to display existing disc
     * types
     */
    void addRowFromDiscType(const cell::DiscType& discType);

    /**
     * @brief Creates a new row with a default disc type, used to add new disc types since we can't make any assumptions
     * about new ones
     */
    void addEmptyRow();

    /**
     * @brief Removes the given row from the model if it exists
     */
    void removeRow(int row);

    /**
     * @brief Deletes all rows and loads the current disc type distribution
     */
    void loadSettings();

    /**
     * @brief Saves the currently edited disc type distributions to the settings, throwing if something is incorrect
     */
    void saveSettings();

    /**
     * @brief Deletes all rows in the model
     */
    void clearRows();

private:
    QVector<std::pair<cell::DiscType, int>> rows_;
};

#endif /* DISCTYPEDISTRIBUTIONTABLEMODEL_HPP */
