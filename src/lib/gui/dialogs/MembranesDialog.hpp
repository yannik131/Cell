#ifndef D4C22358_5E5E_4B84_B28C_76B3039C2F4F_HPP
#define D4C22358_5E5E_4B84_B28C_76B3039C2F4F_HPP

#include "dialogs/TableViewDialog.hpp"
#include "models/MembranesTableModel.hpp"

class MembranesDialog : public TableViewDialog<MembranesTableModel::RowType>
{
    Q_OBJECT
    using Base = TableViewDialog<MembranesTableModel::RowType>;

public:
    MembranesDialog(QWidget* parent, SimulationConfigUpdater* simulationConfigUpdater);
};

#endif /* D4C22358_5E5E_4B84_B28C_76B3039C2F4F_HPP */
