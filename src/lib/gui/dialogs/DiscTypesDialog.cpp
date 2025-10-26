#include "dialogs/DiscTypesDialog.hpp"
#include "core/ColorMapping.hpp"
#include "core/SafeCast.hpp"
#include "delegates/ButtonDelegate.hpp"
#include "delegates/ComboBoxDelegate.hpp"
#include "delegates/SpinBoxDelegate.hpp"
#include "models/DiscTypesTableModel.hpp"

DiscTypesDialog::DiscTypesDialog(QWidget* parent, SimulationConfigUpdater* simulationConfigUpdater)
    : Base(parent, simulationConfigUpdater, new DiscTypesTableModel(this, simulationConfigUpdater))
{
    insertDoubleSpinBoxIntoView(ui->tableView, DoubleSpinBoxParams{.column = 1,
                                                                   .min = cell::DiscTypeLimits::MinRadius,
                                                                   .max = cell::DiscTypeLimits::MaxRadius,
                                                                   .step = 1});
    insertDoubleSpinBoxIntoView(ui->tableView, DoubleSpinBoxParams{.column = 2,
                                                                   .min = cell::DiscTypeLimits::MinMass,
                                                                   .max = cell::DiscTypeLimits::MaxMass,
                                                                   .step = 1});
    insertColorComboBoxIntoView(ui->tableView, Column{3});

    insertDeleteButtonIntoView(model_, ui->tableView, Column{4});
}