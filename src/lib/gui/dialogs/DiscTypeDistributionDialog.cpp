#include "dialogs/DiscTypeDistributionDialog.hpp"
#include "models/DiscTypeDistributionTableModel.hpp"
#include "ui_DiscTypeDistributionDialog.h"

DiscTypeDistributionDialog::DiscTypeDistributionDialog(QWidget* parent,
                                                       AbstractSimulationBuilder* abstractSimulationBuilder)
    : QDialog(parent)
    , ui(new Ui::DiscTypeDistributionDialog)
    , discTypeDistributionTableModel_(new DiscTypeDistributionTableModel(this, abstractSimulationBuilder))
{
    ui->setupUi(this);

    connect(ui->okPushButton, &QPushButton::clicked,
            utility::safeSlot(this,
                              [this]()
                              {
                                  discTypeDistributionTableModel_->commitChanges();
                                  accept();
                              }));
    connect(ui->cancelPushButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(ui->addPushButton, &QPushButton::clicked, discTypeDistributionTableModel_,
            &DiscTypeDistributionTableModel::addEmptyRow);
    connect(ui->clearPushButton, &QPushButton::clicked, discTypeDistributionTableModel_,
            &DiscTypeDistributionTableModel::clearRows);

    insertDiscTypeComboboxIntoView(ui->discTypeDistributionTableView, abstractSimulationBuilder, 0);
    insertProbabilitySpinBoxIntoView(ui->discTypeDistributionTableView, 1);

    insertDeleteButtonIntoView(discTypeDistributionTableModel_, ui->discTypeDistributionTableView, 2);

    ui->discTypeDistributionTableView->setModel(discTypeDistributionTableModel_);
    ui->discTypeDistributionTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void DiscTypeDistributionDialog::showEvent(QShowEvent* event)
{
}
