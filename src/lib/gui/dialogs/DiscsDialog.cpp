#include "dialogs/DiscsDialog.hpp"
#include "delegates/ButtonDelegate.hpp"
#include "delegates/SpinBoxDelegate.hpp"

DiscsDialog::DiscsDialog(QWidget* parent, SimulationConfigUpdater* simulationConfigUpdater)
    : Base(parent, simulationConfigUpdater, new DiscsTableModel(nullptr, simulationConfigUpdater))
{
    insertDiscTypeComboBoxIntoView(ui->tableView, simulationConfigUpdater, 0);
    insertDoubleSpinBoxIntoView(ui->tableView, DoubleSpinBoxParams{.column = 1, .step = 1});
    insertDoubleSpinBoxIntoView(ui->tableView, DoubleSpinBoxParams{.column = 2, .step = 1});
    insertDoubleSpinBoxIntoView(ui->tableView, DoubleSpinBoxParams{.column = 3, .step = 1});
    insertDoubleSpinBoxIntoView(ui->tableView, DoubleSpinBoxParams{.column = 4, .step = 1});
    insertDeleteButtonIntoView(model_, ui->tableView, Column{5});
}