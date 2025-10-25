#ifndef A9DE5EDF_968A_4196_840D_F282E2521335_HPP
#define A9DE5EDF_968A_4196_840D_F282E2521335_HPP

#include "dialogs/TableViewDialog.hpp"
#include "models/DiscTypeDistributionTableModel.hpp"

class DiscTypeDistributionDialog : public TableViewDialog<DiscTypeDistributionTableModel::RowType>
{
    Q_OBJECT
    using Base = TableViewDialog<DiscTypeDistributionTableModel::RowType>;

public:
    DiscTypeDistributionDialog(QWidget* parent, SimulationConfigUpdater* simulationConfigUpdater);
};

#endif /* A9DE5EDF_968A_4196_840D_F282E2521335_HPP */
