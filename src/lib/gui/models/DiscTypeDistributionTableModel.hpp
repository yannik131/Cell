#ifndef C5A7F999_3DE1_4DFF_8512_AD7C6C768848_HPP
#define C5A7F999_3DE1_4DFF_8512_AD7C6C768848_HPP

#include "models/AbstractSimulationConfigTableModel.hpp"

class DiscTypeDistributionTableModel : public AbstractSimulationConfigTableModel<DiscTypeDistributionEntry>
{
    Q_OBJECT
public:
    DiscTypeDistributionTableModel(QObject* parent, SimulationConfigUpdater* simulationConfigUpdater);

    void addRow() override;

private:
    QVariant getField(const DiscTypeDistributionEntry& row, const QModelIndex& index) const override;
    bool setField(DiscTypeDistributionEntry& row, const QModelIndex& index, const QVariant& value) override;
    bool isEditable(const QModelIndex& index) const override;
};

#endif /* C5A7F999_3DE1_4DFF_8512_AD7C6C768848_HPP */
