#ifndef A3747B40_07FD_4BC6_92CC_11E34A48E3A5_HPP
#define A3747B40_07FD_4BC6_92CC_11E34A48E3A5_HPP

#include "dialogs/TableViewDialog.hpp"
#include "models/DiscsTableModel.hpp"

class DiscsDialog : public TableViewDialog<DiscsTableModel::RowType>
{
    Q_OBJECT
    using Base = TableViewDialog<DiscsTableModel::RowType>;

public:
    DiscsDialog(QWidget* parent, SimulationConfigUpdater* simulationConfigUpdater);
};

#endif /* A3747B40_07FD_4BC6_92CC_11E34A48E3A5_HPP */
