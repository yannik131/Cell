#ifndef DD933800_4FD0_4D74_8399_33BE7E519C69_HPP
#define DD933800_4FD0_4D74_8399_33BE7E519C69_HPP

#include "cell/SimulationConfig.hpp"

#include <QAbstractTableModel>
#include <QVector>

#include <SFML/Graphics/Color.hpp>

#include <unordered_set>

class AbstractSimulationBuilder;

class MembraneTypesTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit MembraneTypesTableModel(QObject* parent, AbstractSimulationBuilder* abstractSimulationBuilder);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    /**
     * @returns 5: Name, radius, color, permeabilities, delete
     */
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    bool setData(const QModelIndex& index, const QVariant& value, int role) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    void addEmptyRow();

    void removeRow(int row);

    void clearRows();

    void commitChanges();

    void reload();

    cell::config::MembraneType& getRow(int row);

private:
    void updateMembraneTypeName(const std::string& newName, int row);

private:
    std::vector<cell::config::MembraneType> rows_;
    std::map<std::string, sf::Color> membraneTypeColorMap_;
    std::unordered_set<std::string> removedMembraneTypes_;

    AbstractSimulationBuilder* abstractSimulationBuilder_;
};

#endif /* DD933800_4FD0_4D74_8399_33BE7E519C69_HPP */
