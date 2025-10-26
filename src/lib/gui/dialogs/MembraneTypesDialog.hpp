#ifndef C8FA243B_5843_4BD9_9E3A_0C6297CED23C_HPP
#define C8FA243B_5843_4BD9_9E3A_0C6297CED23C_HPP

#include "dialogs/TableViewDialog.hpp"
#include "models/MembraneTypesTableModel.hpp"

class MembraneTypesTableModel;
class PermeabilityDialog;
class DiscTypeDistributionDialog;

class MembraneTypesDialog : public TableViewDialog<MembraneTypesTableModel::RowType>
{
    Q_OBJECT
    using Base = TableViewDialog<MembraneTypesTableModel::RowType>;

public:
    MembraneTypesDialog(QWidget* parent, SimulationConfigUpdater* simulationConfigUpdater);

private:
    PermeabilityDialog* permeabilityDialog_;
    DiscTypeDistributionDialog* discTypeDistributionDialog_;
    int selectedRowForPermeabilityEditing_;
    int selectedRowForDistributionEditing_;
};

#endif /* C8FA243B_5843_4BD9_9E3A_0C6297CED23C_HPP */
