#ifndef D7A2ACDF_B39A_4AA7_9906_03973244209E_HPP
#define D7A2ACDF_B39A_4AA7_9906_03973244209E_HPP

#include "models/AbstractSimulationConfigPartTableModel.hpp"

class PermeabilityTableModel
    : public AbstractSimulationConfigPartTableModel<std::pair<std::string, cell::MembraneType::Permeability>>
{
    Q_OBJECT
public:
    PermeabilityTableModel(QObject* parent, SimulationConfigUpdater* simulationConfigUpdater);

    void addRow() override;

private:
    QVariant getField(const std::pair<std::string, cell::MembraneType::Permeability>& row, int column) const override;
    bool setField(std::pair<std::string, cell::MembraneType::Permeability>& row, int column,
                  const QVariant& value) override;
    bool isEditable(const QModelIndex& index) const override;
};

#endif /* D7A2ACDF_B39A_4AA7_9906_03973244209E_HPP */
