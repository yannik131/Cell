#include "dialogs/DiscTypesDialog.hpp"
#include "cell/Settings.hpp"
#include "core/ColorMapping.hpp"
#include "core/SafeCast.hpp"
#include "delegates/ButtonDelegate.hpp"
#include "delegates/ComboBoxDelegate.hpp"
#include "delegates/SpinBoxDelegate.hpp"
#include "ui_DiscTypesDialog.h"

#include "DiscTypesDialog.hpp"
#include <QCloseEvent>
#include <QMessageBox>

DiscTypesDialog::DiscTypesDialog(QWidget* parent, AbstractSimulationBuilder* abstractSimulationBuilder)
    : QDialog(parent)
    , ui(new Ui::DiscTypesDialog)
    , discTypesTableModel_(new DiscTypesTableModel(this, abstractSimulationBuilder))
{
    ui->setupUi(this);

    connect(ui->okPushButton, &QPushButton::clicked,
            [this]()
            {
                discTypesTableModel_->commitChanges();
                hide();
            });
    connect(ui->cancelPushButton, &QPushButton::clicked, this, &DiscTypesDialog::cancel);
    connect(ui->addTypePushButton, &QPushButton::clicked, discTypesTableModel_, &DiscTypesTableModel::addEmptyRow);
    connect(ui->clearTypesPushButton, &QPushButton::clicked, discTypesTableModel_, &DiscTypesTableModel::clearRows);

    auto* colorComboBoxDelegate = new ComboBoxDelegate(this);
    auto* radiusSpinBoxDelegate = new SpinBoxDelegate<QDoubleSpinBox>(this);
    auto* massSpinBoxDelegate = new SpinBoxDelegate<QDoubleSpinBox>(this);
    auto* deleteButtonDelegate = new ButtonDelegate(this, "Delete");

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
    connect(deleteButtonDelegate, &ButtonDelegate::buttonClicked, discTypesTableModel_,
            &DiscTypesTableModel::removeRow);

    ui->discTypesTableView->setItemDelegateForColumn(1, radiusSpinBoxDelegate);
    ui->discTypesTableView->setItemDelegateForColumn(2, massSpinBoxDelegate);
    ui->discTypesTableView->setItemDelegateForColumn(3, colorComboBoxDelegate);
    ui->discTypesTableView->setItemDelegateForColumn(4, deleteButtonDelegate);
    ui->discTypesTableView->setEditTriggers(QAbstractItemView::EditTrigger::CurrentChanged |
                                            QAbstractItemView::EditTrigger::SelectedClicked);

    ui->discTypesTableView->setModel(discTypesTableModel_);
}

void DiscTypesDialog::closeEvent(QCloseEvent*)
{
    discTypesTableModel_->discardChanges();
}

void DiscTypesDialog::cancel()
{
    discTypesTableModel_->discardChanges();
    hide();
}
