#include "dialogs/DiscTypesDialog.hpp"
#include "core/ColorMapping.hpp"
#include "core/SafeCast.hpp"
#include "delegates/ButtonDelegate.hpp"
#include "delegates/ComboBoxDelegate.hpp"
#include "delegates/SpinBoxDelegate.hpp"
#include "models/DiscTypesTableModel.hpp"

DiscTypesDialog::DiscTypesDialog(QWidget* parent, SimulationConfigUpdater* simulationConfigUpdater)
    : Base(parent, simulationConfigUpdater, new DiscTypesTableModel(nullptr, simulationConfigUpdater))
{
    insertDoubleSpinBoxIntoView(ui->tableView, DoubleSpinBoxParams{.column = 1, .step = 1, .decimals = 3});
    insertDoubleSpinBoxIntoView(ui->tableView, DoubleSpinBoxParams{.column = 2, .step = 1, .decimals = 3});
    insertColorComboBoxIntoView(ui->tableView, Column{3});

    insertDeleteButtonIntoView(model_, ui->tableView, Column{4});
}