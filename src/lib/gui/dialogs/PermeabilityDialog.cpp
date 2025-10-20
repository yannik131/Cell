#include "core/SafeCast.hpp"
#include "core/Utility.hpp"
#include "delegates/ComboBoxDelegate.hpp"
#include "dialogs/DiscTypesDialog.hpp"
#include "models/PermeabilityTableModel.hpp"
#include "ui_PermeabilityDialog.h"

#include "PermeabilityDialog.hpp"
#include <QCloseEvent>
#include <QMessageBox>

PermeabilityDialog::PermeabilityDialog(QWidget* parent, AbstractSimulationBuilder* abstractSimulationBuilder)
    : QDialog(parent)
    , ui(new Ui::PermeabilityDialog)
    , permeabilityTableModel_(new PermeabilityTableModel(this, abstractSimulationBuilder))
{
    ui->setupUi(this);

    connect(ui->okPushButton, &QPushButton::clicked,
            utility::safeSlot(this,
                              [this]()
                              {
                                  permeabilityTableModel_->commitChanges();
                                  accept();
                              }));
    connect(ui->cancelPushButton, &QPushButton::clicked, this, &QDialog::reject);

    ui->permeabilityTableView->setItemDelegateForColumn(3, colorComboBoxDelegate);

    auto* permeabilityComboBoxDelegate = new ComboBoxDelegate(this);

    connect(permeabilityComboBoxDelegate, &ComboBoxDelegate::editorCreated,
            [](QComboBox* comboBox) { comboBox->addItems({"None", "Inward", "Outward", "Bidirectional"}); });

    ui->permeabilityTableView->setEditTriggers(QAbstractItemView::EditTrigger::CurrentChanged |
                                               QAbstractItemView::EditTrigger::SelectedClicked);
    ui->permeabilityTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->permeabilityTableView->setModel(permeabilityTableModel_);
}

void PermeabilityDialog::setPermeabilityMap(
    std::unordered_map<std::string, cell::MembraneType::Permeability>& permeabilityMap)
{
    permeabilityTableModel_->setPermeabilityMap(permeabilityMap);
}

void PermeabilityDialog::showEvent(QShowEvent*)
{
    permeabilityTableModel_->reload();
}
