#include "dialogs/DiscTypeDistributionDialog.hpp"
#include "DiscTypeDistributionDialog.hpp"
#include "core/Utility.hpp"
#include "delegates/ButtonDelegate.hpp"
#include "delegates/ComboBoxDelegate.hpp"
#include "delegates/SpinBoxDelegate.hpp"

DiscTypeDistributionDialog::DiscTypeDistributionDialog(QWidget* parent,
                                                       SimulationConfigUpdater* simulationConfigUpdater)
    : Base(parent, simulationConfigUpdater, new DiscTypeDistributionTableModel(nullptr, simulationConfigUpdater))
{
    insertDiscTypeComboBoxIntoView(ui->tableView, simulationConfigUpdater, 0);
    insertProbabilitySpinBoxIntoView(ui->tableView, Column{1});

    insertDeleteButtonIntoView(model_, ui->tableView, Column{2});
}

void DiscTypeDistributionDialog::setDiscTypeDistribution(const DiscTypeDistribution& discTypeDistribution)
{
    std::vector<DiscTypeDistributionEntry> rows(discTypeDistribution.begin(), discTypeDistribution.end());
    model_->setRows(std::move(rows));
}

DiscTypeDistribution DiscTypeDistributionDialog::getDiscTypeDistribution() const
{
    const auto& rows = model_->getRows();
    DiscTypeDistribution distribution(rows.begin(), rows.end());

    return distribution;
}
