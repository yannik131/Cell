#ifndef FF484939_FF4B_43EC_AAEA_E6416E44A6F1_HPP
#define FF484939_FF4B_43EC_AAEA_E6416E44A6F1_HPP

#include "dialogs/TableViewDialog.hpp"
#include "models/ReactionsTableModel.hpp"

class ReactionsDialog : public TableViewDialog<ReactionsTableModel::RowType>
{
    Q_OBJECT
    using Base = TableViewDialog<ReactionsTableModel::RowType>;

public:
    ReactionsDialog(QWidget* parent, SimulationConfigUpdater* simulationConfigUpdater);
};

#endif /* FF484939_FF4B_43EC_AAEA_E6416E44A6F1_HPP */
