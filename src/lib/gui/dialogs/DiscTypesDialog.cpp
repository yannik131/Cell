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
    auto* colorComboBoxDelegate = new ComboBoxDelegate(this);
    auto* radiusSpinBoxDelegate = new SpinBoxDelegate<QDoubleSpinBox>(this);
    auto* massSpinBoxDelegate = new SpinBoxDelegate<QDoubleSpinBox>(this);

    connect(colorComboBoxDelegate, &ComboBoxDelegate::editorCreated,
            [](QComboBox* comboBox) { comboBox->addItems(getSupportedDiscColorNames()); });
    connect(radiusSpinBoxDelegate, &SpinBoxDelegate<QDoubleSpinBox>::editorCreated,
            [](QWidget* spinBox)
            {
                safeCast<QDoubleSpinBox*>(spinBox)->setRange(cell::DiscTypeLimits::MinRadius,
                                                             cell::DiscTypeLimits::MaxRadius);
            });
    connect(massSpinBoxDelegate, &SpinBoxDelegate<QDoubleSpinBox>::editorCreated,
            [](QWidget* spinBox)
            {
                safeCast<QDoubleSpinBox*>(spinBox)->setRange(cell::DiscTypeLimits::MinMass,
                                                             cell::DiscTypeLimits::MaxMass);
            });

    ui->tableView->setItemDelegateForColumn(1, radiusSpinBoxDelegate);
    ui->tableView->setItemDelegateForColumn(2, massSpinBoxDelegate);
    ui->tableView->setItemDelegateForColumn(3, colorComboBoxDelegate);
    insertDeleteButtonIntoView(model_, ui->tableView, 4);
}