#ifndef DD933800_4FD0_4D74_8399_33BE7E519C69_HPP
#define DD933800_4FD0_4D74_8399_33BE7E519C69_HPP

#include "cell/SimulationConfig.hpp"
#include "models/AbstractSimulationConfigTableModel.hpp"

#include <SFML/Graphics/Color.hpp>

#include <unordered_set>

class AbstractSimulationBuilder;

class MembraneTypesTableModel : public AbstractSimulationConfigTableModel<cell::config::MembraneType>,
                                public AbstractConfigChanger
{
    Q_OBJECT
    using Base = AbstractSimulationConfigTableModel<cell::config::MembraneType>;

public:
    MembraneTypesTableModel(QObject* parent, SimulationConfigUpdater* simulationConfigUpdater);

    void removeRow(int row) override;
    void clearRows() override;
    void addRow() override;
    void loadFromConfig() override;
    void saveToConfig() override;

private:
    QVariant getField(const cell::config::MembraneType& row, int column) const override;
    bool setField(cell::config::MembraneType& row, int column, const QVariant& value) override;
    bool isEditable(const QModelIndex& index) const override;
    void updateMembraneTypeName(cell::config::MembraneType& membraneType, const std::string& newName);

private:
    std::map<std::string, sf::Color> membraneTypeColorMap_;
    std::unordered_set<std::string> removedMembraneTypes_;
};

#endif /* DD933800_4FD0_4D74_8399_33BE7E519C69_HPP */
