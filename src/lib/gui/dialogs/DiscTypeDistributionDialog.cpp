#include "dialogs/DiscTypeDistributionDialog.hpp"
#include "core/Utility.hpp"
#include "delegates/ButtonDelegate.hpp"
#include "delegates/ComboBoxDelegate.hpp"
#include "delegates/SpinBoxDelegate.hpp"

DiscTypeDistributionDialog::DiscTypeDistributionDialog(QWidget* parent,
                                                       SimulationConfigUpdater* simulationConfigUpdater)
    : Base(parent, simulationConfigUpdater, new DiscTypeDistributionTableModel(this, simulationConfigUpdater))
{
    insertDiscTypeComboboxIntoView(ui->tableView, simulationConfigUpdater, 0);
    insertProbabilitySpinBoxIntoView(ui->tableView, 1);

    insertDeleteButtonIntoView(model_, ui->tableView, 2);
}