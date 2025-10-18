#include "dialogs/MembraneTypesDialog.hpp"
#include "core/ColorMapping.hpp"
#include "core/Utility.hpp"
#include "delegates/ButtonDelegate.hpp"
#include "delegates/ComboBoxDelegate.hpp"
#include "delegates/SpinBoxDelegate.hpp"
#include "dialogs/PermeabilityDialog.hpp"
#include "models/MembraneTypesTableModel.hpp"
#include "ui_MembraneTypesDialog.h"

MembraneTypesDialog::MembraneTypesDialog(QWidget* parent, AbstractSimulationBuilder* abstractSimulationBuilder)
    : QDialog(parent)
    , ui(new Ui::MembraneTypesDialog)
    , membraneTypesTableModel_(new MembraneTypesTableModel(this, abstractSimulationBuilder))
    , permeabilityDialog_(new PermeabilityDialog(this, abstractSimulationBuilder))
{
    ui->setupUi(this);

    connect(ui->okPushButton, &QPushButton::clicked,
            utility::safeSlot(this,
                              [this]()
                              {
                                  membraneTypesTableModel_->commitChanges();
                                  accept();
                              }));

    connect(ui->cancelPushButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(ui->addMembranePushButton, &QPushButton::clicked, membraneTypesTableModel_,
            &MembraneTypesTableModel::addEmptyRow);
    connect(ui->clearMembranesPushButton, &QPushButton::clicked, membraneTypesTableModel_,
            &MembraneTypesTableModel::clearRows);

    auto* radiusSpinBoxDelegate = new SpinBoxDelegate<QDoubleSpinBox>(ui->membraneTypesTableView);
    auto* colorComboBoxDelegate = new ComboBoxDelegate(ui->membraneTypesTableView);
    auto* editPermeabilityPushButtonDelegate = new ButtonDelegate(ui->membraneTypesTableView, "Edit");

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
                auto& permeabilityMap = membraneTypesTableModel_->getRow(row).permeabilityMap;
                permeabilityDialog_->setPermeabilityMap(permeabilityMap);
                permeabilityDialog_->show();
            });

    ui->membraneTypesTableView->setItemDelegateForColumn(1, radiusSpinBoxDelegate);
    ui->membraneTypesTableView->setItemDelegateForColumn(2, colorComboBoxDelegate);
    ui->membraneTypesTableView->setItemDelegateForColumn(3, editPermeabilityPushButtonDelegate);
    insertDeleteButtonIntoView(membraneTypesTableModel_, ui->membraneTypesTableView, 4);

    ui->membraneTypesTableView->setEditTriggers(QAbstractItemView::EditTrigger::CurrentChanged |
                                                QAbstractItemView::EditTrigger::SelectedClicked);
    ui->membraneTypesTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->membraneTypesTableView->setModel(membraneTypesTableModel_);
}

void MembraneTypesDialog::showEvent(QShowEvent* event)
{
    membraneTypesTableModel_->reload();
}