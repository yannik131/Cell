#include "dialogs/MembraneTypesDialog.hpp"
#include "core/ColorMapping.hpp"
#include "core/Utility.hpp"
#include "delegates/ButtonDelegate.hpp"
#include "delegates/ComboBoxDelegate.hpp"
#include "delegates/SpinBoxDelegate.hpp"
#include "dialogs/PermeabilityDialog.hpp"
#include "models/MembraneTypesTableModel.hpp"

MembraneTypesDialog::MembraneTypesDialog(QWidget* parent, SimulationConfigUpdater* simulationConfigUpdater)
    : Base(parent, simulationConfigUpdater, new MembraneTypesTableModel(this, simulationConfigUpdater))
    , permeabilityDialog_(new PermeabilityDialog(this, simulationConfigUpdater))
{
    auto* radiusSpinBoxDelegate = new SpinBoxDelegate<QDoubleSpinBox>(ui->tableView);
    auto* colorComboBoxDelegate = new ComboBoxDelegate(ui->tableView);
    auto* editPermeabilityPushButtonDelegate = new ButtonDelegate(ui->tableView, "Edit");

    connect(radiusSpinBoxDelegate, &SpinBoxDelegate<QDoubleSpinBox>::editorCreated,
            [](QWidget* spinBox)
            {
                safeCast<QDoubleSpinBox*>(spinBox)->setRange(cell::MembraneTypeLimits::MinRadius,
                                                             cell::MembraneTypeLimits::MaxRadius);
            });
    connect(colorComboBoxDelegate, &ComboBoxDelegate::editorCreated,
            [](QComboBox* comboBox) { comboBox->addItems(getSupportedDiscColorNames()); });
    connect(editPermeabilityPushButtonDelegate, &ButtonDelegate::buttonClicked, this,
            [&](int row)
            {
                selectedRowForPermeabilityEditing_ = row;
                auto permeabilityMap = model_->getRows()[row].permeabilityMap;
                permeabilityDialog_->setPermeabilityMap(permeabilityMap);
                permeabilityDialog_->show();
            });

    ui->tableView->setItemDelegateForColumn(1, radiusSpinBoxDelegate);
    ui->tableView->setItemDelegateForColumn(2, colorComboBoxDelegate);
    ui->tableView->setItemDelegateForColumn(3, editPermeabilityPushButtonDelegate);
    insertDeleteButtonIntoView(model_, ui->tableView, 4);
}
