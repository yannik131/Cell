#ifndef AB3F4088_AF2F_4472_8BA3_E8165A0B4B30_HPP
#define AB3F4088_AF2F_4472_8BA3_E8165A0B4B30_HPP

#include "models/AbstractSimulationConfigTableModel.hpp"

class DiscsTableModel : public AbstractSimulationConfigTableModel<cell::config::Disc>, public AbstractConfigChanger
{
    Q_OBJECT
public:
    DiscsTableModel(QObject* parent, SimulationConfigUpdater* simulationConfigUpdater);

    void addRow() override;
    void loadFromConfig() override;
    void saveToConfig() override;

private:
    QVariant getField(const cell::config::Disc& row, const QModelIndex& index) const override;
    bool setField(cell::config::Disc& row, const QModelIndex& index, const QVariant& value) override;
    bool isEditable(const QModelIndex& index) const override;
};

#endif /* AB3F4088_AF2F_4472_8BA3_E8165A0B4B30_HPP */
