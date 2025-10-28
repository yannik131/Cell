#include "dialogs/MembranesDialog.hpp"
#include "core/SimulationConfigUpdater.hpp"
#include "delegates/ButtonDelegate.hpp"
#include "delegates/ComboBoxDelegate.hpp"
#include "delegates/SpinBoxDelegate.hpp"

MembranesDialog::MembranesDialog(QWidget* parent, SimulationConfigUpdater* simulationConfigUpdater)
    : Base(parent, simulationConfigUpdater, new MembranesTableModel(nullptr, simulationConfigUpdater))
{
    insertMembraneTypeComboBoxIntoView(ui->tableView, simulationConfigUpdater, Column{0});

    const auto cellRadius = simulationConfigUpdater->getSimulationConfig().cellMembraneType.radius;
    insertDoubleSpinBoxIntoView(ui->tableView,
                                DoubleSpinBoxParams{.column = 1, .min = -cellRadius, .max = cellRadius, .step = 1});
    insertDoubleSpinBoxIntoView(ui->tableView,
                                DoubleSpinBoxParams{.column = 2, .min = -cellRadius, .max = cellRadius, .step = 1});

    insertDeleteButtonIntoView(model_, ui->tableView, Column{3});
}