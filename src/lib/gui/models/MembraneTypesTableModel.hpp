#ifndef DD933800_4FD0_4D74_8399_33BE7E519C69_HPP
#define DD933800_4FD0_4D74_8399_33BE7E519C69_HPP

#include "cell/SimulationConfig.hpp"
#include "models/AbstractSimulationConfigTableModel.hpp"

#include <SFML/Graphics/Color.hpp>

#include <unordered_set>

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
    QVariant getField(const cell::config::MembraneType& row, const QModelIndex& index) const override;
    bool setField(cell::config::MembraneType& row, const QModelIndex& index, const QVariant& value) override;
    bool isEditable(const QModelIndex& index) const override;
    bool isEnabled(const QModelIndex& index) const override;

private:
    std::vector<sf::Color> membraneColors_; // TODO DRY violation with DiscTypesTableModel (colors and original names)
    std::vector<std::string> originalMembraneTypeNames_;
    std::unordered_set<std::string> removedMembraneTypes_;
};

#endif /* DD933800_4FD0_4D74_8399_33BE7E519C69_HPP */
