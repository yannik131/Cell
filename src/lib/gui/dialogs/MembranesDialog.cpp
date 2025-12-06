#include "dialogs/MembranesDialog.hpp"
#include "core/SimulationConfigUpdater.hpp"
#include "delegates/ButtonDelegate.hpp"
#include "delegates/ComboBoxDelegate.hpp"
#include "delegates/SpinBoxDelegate.hpp"

MembranesDialog::MembranesDialog(QWidget* parent, SimulationConfigUpdater* simulationConfigUpdater)
    : Base(parent, simulationConfigUpdater, new MembranesTableModel(nullptr, simulationConfigUpdater))
{
    insertMembraneTypeComboBoxIntoView(ui->tableView, simulationConfigUpdater, Column{0});

    insertDoubleSpinBoxIntoView(ui->tableView, DoubleSpinBoxParams{.column = 1, .step = 1});
    insertDoubleSpinBoxIntoView(ui->tableView, DoubleSpinBoxParams{.column = 2, .step = 1});

    insertDeleteButtonIntoView(model_, ui->tableView, Column{3});
}