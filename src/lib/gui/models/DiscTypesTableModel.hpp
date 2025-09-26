#ifndef B820F75C_97AE_4E27_9359_BBAADC72724F_HPP
#define B820F75C_97AE_4E27_9359_BBAADC72724F_HPP

#include "cell/SimulationConfig.hpp"

#include <QAbstractTableModel>
#include <QVector>

#include <SFML/Graphics/Color.hpp>

#include <unordered_set>

class AbstractSimulationBuilder;

class DiscTypesTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    /**
     * @brief Creates the model with the injected `AbstractSimulationBuilder` providing direct access to the config
     */
    explicit DiscTypesTableModel(QObject* parent, AbstractSimulationBuilder* abstractSimulationBuilder);

    /**
     * @returns Number of disc types
     */
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    /**
     * @returns 5: Disc type name, radius, mass, color, delete button
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
     * @brief All columns can be edited here
     */
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    /**
     * @brief Creates a new row with a default disc type
     */
    void addEmptyRow();

    /**
     * @brief Removes the given row from the model if it exists
     */
    void removeRow(int row);

    /**
     * @brief Deletes all rows in the model
     */
    void clearRows();

    /**
     * @brief Sets the disc types using the injected dependency
     */
    void commitChanges();

    /**
     * @brief Reloads the disc types from the injected dependency
     */
    void reload();

private:
    void updateDiscTypeName(const std::string& newName, int row);

private:
    std::vector<cell::config::DiscType> rows_;
    std::map<std::string, sf::Color> discTypeColorMap_;
    std::unordered_set<std::string> removedDiscTypes_;

    AbstractSimulationBuilder* abstractSimulationBuilder_;
};

#endif /* B820F75C_97AE_4E27_9359_BBAADC72724F_HPP */
