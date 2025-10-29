#include "dialogs/PermeabilityDialog.hpp"
#include "core/SafeCast.hpp"
#include "core/Utility.hpp"
#include "delegates/ButtonDelegate.hpp"
#include "delegates/ComboBoxDelegate.hpp"
#include "models/PermeabilityTableModel.hpp"

#include <QCloseEvent>
#include <QMessageBox>

PermeabilityDialog::PermeabilityDialog(QWidget* parent, SimulationConfigUpdater* simulationConfigUpdater)
    : Base(parent, simulationConfigUpdater, new PermeabilityTableModel(nullptr, simulationConfigUpdater))
{
    insertDiscTypeComboBoxIntoView(ui->tableView, simulationConfigUpdater, 0);
    insertComboBoxIntoView(ui->tableView, Column{1}, {"None", "Inward", "Outward", "Bidirectional"});
    insertDeleteButtonIntoView(model_, ui->tableView, Column{2});
}

void PermeabilityDialog::setPermeabilityMap(const PermeabilityMap& permeabilityMap)
{
    std::vector<PermeabilityMapEntry> rows(permeabilityMap.begin(), permeabilityMap.end());
    model_->setRows(std::move(rows));
}

PermeabilityMap PermeabilityDialog::getPermeabilityMap() const
{
    const auto& rows = model_->getRows();
    PermeabilityMap permeabilityMap(rows.begin(), rows.end());

    return permeabilityMap;
}
