#include "dialogs/DiscTypesDialog.hpp"
#include "cell/Settings.hpp"
#include "core/ColorMapping.hpp"
#include "core/SafeCast.hpp"
#include "core/Utility.hpp"
#include "delegates/ButtonDelegate.hpp"
#include "delegates/ComboBoxDelegate.hpp"
#include "delegates/SpinBoxDelegate.hpp"
#include "models/DiscTypesTableModel.hpp"
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
            utility::safeSlot(this,
                              [this]()
                              {
                                  discTypesTableModel_->commitChanges();
                                  accept();
                              }));
    connect(ui->cancelPushButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(ui->addTypePushButton, &QPushButton::clicked, discTypesTableModel_, &DiscTypesTableModel::addEmptyRow);
    connect(ui->clearTypesPushButton, &QPushButton::clicked, discTypesTableModel_, &DiscTypesTableModel::clearRows);

    auto* colorComboBoxDelegate = new ComboBoxDelegate(this);
    auto* radiusSpinBoxDelegate = new SpinBoxDelegate<QDoubleSpinBox>(this);
    auto* massSpinBoxDelegate = new SpinBoxDelegate<QDoubleSpinBox>(this);

    insertDeleteButtonIntoView(this, discTypesTableModel_, ui->discTypesTableView, 4);

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

    ui->discTypesTableView->setItemDelegateForColumn(1, radiusSpinBoxDelegate);
    ui->discTypesTableView->setItemDelegateForColumn(2, massSpinBoxDelegate);
    ui->discTypesTableView->setItemDelegateForColumn(3, colorComboBoxDelegate);
    ui->discTypesTableView->setEditTriggers(QAbstractItemView::EditTrigger::CurrentChanged |
                                            QAbstractItemView::EditTrigger::SelectedClicked);
    ui->discTypesTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->discTypesTableView->setModel(discTypesTableModel_);
}

void DiscTypesDialog::showEvent(QShowEvent* event)
{
    discTypesTableModel_->reload();
}
