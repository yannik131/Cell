#ifndef EBD19D22_0EC6_4D52_8BBA_73271A5FE110_HPP
#define EBD19D22_0EC6_4D52_8BBA_73271A5FE110_HPP

#include "dialogs/TableViewDialog.hpp"
#include "models/DiscTypesTableModel.hpp"

class DiscTypesDialog : public TableViewDialog<DiscTypesTableModel::RowType>
{
    Q_OBJECT
    using Base = TableViewDialog<DiscTypesTableModel::RowType>;

public:
    DiscTypesDialog(QWidget* parent, SimulationConfigUpdater* simulationConfigUpdater);
};

#endif /* EBD19D22_0EC6_4D52_8BBA_73271A5FE110_HPP */
