#ifndef C914B4C9_59E7_4A19_90BC_AB93E0A68106_HPP
#define C914B4C9_59E7_4A19_90BC_AB93E0A68106_HPP

#include "cell/Reaction.hpp"
#include "cell/SimulationConfig.hpp"
#include "models/AbstractSimulationConfigTableModel.hpp"

class ReactionsTableModel : public AbstractSimulationConfigTableModel<cell::config::Reaction>
{
    Q_OBJECT
public:
    ReactionsTableModel(QObject* parent, SimulationConfigUpdater* simulationConfigUpdater);

    void addRow() override
    {
        throw ExceptionWithLocation("Use the other addRow pls");
    };
    void addRow(cell::Reaction::Type type);
    void loadFromConfig() override;
    void saveToConfig() override;

private:
    QVariant getField(const cell::config::Reaction& row, int column) const override;
    bool setField(cell::config::Reaction& row, int column, const QVariant& value) override;
    bool isEditable(const QModelIndex& index) const override;
};

#endif /* C914B4C9_59E7_4A19_90BC_AB93E0A68106_HPP */
