#include "dialogs/DiscsDialog.hpp"
#include "delegates/ButtonDelegate.hpp"
#include "delegates/SpinBoxDelegate.hpp"

DiscsDialog::DiscsDialog(QWidget* parent, SimulationConfigUpdater* simulationConfigUpdater)
    : Base(parent, simulationConfigUpdater, new DiscsTableModel(nullptr, simulationConfigUpdater))
{
    insertDiscTypeComboBoxIntoView(ui->tableView, simulationConfigUpdater, 0);
    const auto cellRadius = simulationConfigUpdater->getSimulationConfig().cellMembraneType.radius;
    insertDoubleSpinBoxIntoView(ui->tableView,
                                DoubleSpinBoxParams{.column = 1, .min = -cellRadius, .max = cellRadius, .step = 1});
    insertDoubleSpinBoxIntoView(ui->tableView,
                                DoubleSpinBoxParams{.column = 2, .min = -cellRadius, .max = cellRadius, .step = 1});
    insertDoubleSpinBoxIntoView(ui->tableView, DoubleSpinBoxParams{.column = 3, .min = -1000, .max = 1000, .step = 1});
    insertDoubleSpinBoxIntoView(ui->tableView, DoubleSpinBoxParams{.column = 4, .min = -1000, .max = 1000, .step = 1});
    insertDeleteButtonIntoView(model_, ui->tableView, Column{5});
}