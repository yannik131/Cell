#ifndef C5A7F999_3DE1_4DFF_8512_AD7C6C768848_HPP
#define C5A7F999_3DE1_4DFF_8512_AD7C6C768848_HPP

#include "models/AbstractSimulationConfigPartTableModel.hpp"

class DiscTypeDistributionTableModel : public AbstractSimulationConfigPartTableModel<std::pair<std::string, double>>
{
    Q_OBJECT
public:
    DiscTypeDistributionTableModel(QObject* parent, SimulationConfigUpdater* simulationConfigUpdater);

    void addRow() override;

private:
    QVariant getField(const std::pair<std::string, double>& row, int column) const override;
    bool setField(std::pair<std::string, double>& row, int column, const QVariant& value) override;
    bool isEditable(const QModelIndex& index) const override;
};

#endif /* C5A7F999_3DE1_4DFF_8512_AD7C6C768848_HPP */
