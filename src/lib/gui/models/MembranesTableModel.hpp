#ifndef C1D94502_857B_4E2A_9378_0F448C9C4303_HPP
#define C1D94502_857B_4E2A_9378_0F448C9C4303_HPP

#include "cell/SimulationConfig.hpp"
#include "models/AbstractSimulationConfigTableModel.hpp"

class MembranesTableModel : public AbstractSimulationConfigTableModel<cell::config::Membrane>,
                            public AbstractConfigChanger
{
    Q_OBJECT
public:
    MembranesTableModel(QObject* parent, SimulationConfigUpdater* simulationConfigUpdater);

    void addRow() override;
    void loadFromConfig() override;
    void saveToConfig() override;

private:
    QVariant getField(const cell::config::Membrane& row, const QModelIndex& index) const override;
    bool setField(cell::config::Membrane& row, const QModelIndex& index, const QVariant& value) override;
    bool isEditable(const QModelIndex& index) const override;
};

#endif /* C1D94502_857B_4E2A_9378_0F448C9C4303_HPP */
