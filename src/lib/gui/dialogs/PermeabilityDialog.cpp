#include "core/SafeCast.hpp"
#include "core/Utility.hpp"
#include "delegates/ComboBoxDelegate.hpp"
#include "dialogs/DiscTypesDialog.hpp"
#include "models/PermeabilityTableModel.hpp"
#include "ui_PermeabilityDialog.h"

#include <QCloseEvent>
#include <QMessageBox>

PermeabilityDialog::PermeabilityDialog(QWidget* parent, AbstractSimulationBuilder* abstractSimulationBuilder,
                                       PermeabilityTableModel* permeabilityTableModel)
    : QDialog(parent)
    , ui(new Ui::PermeabilityDialog)
    , permeabilityTableModel_(permeabilityTableModel)
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

void DiscTypesDialog::showEvent(QShowEvent*)
{
    permeabilityTableModel_->reload();
}
