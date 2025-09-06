#include "dialogs/ReactionsDialog.hpp"
#include "core/SafeCast.hpp"
#include "core/Utility.hpp"
#include "delegates/ButtonDelegate.hpp"
#include "delegates/ComboBoxDelegate.hpp"
#include "delegates/SpinBoxDelegate.hpp"
#include "models/ReactionsTableModel.hpp"
#include "ui_ReactionsDialog.h"

#include <QCloseEvent>
#include <QMessageBox>

ReactionsDialog::ReactionsDialog(QWidget* parent, AbstractSimulationBuilder* abstractSimulationBuilder)
    : QDialog(parent)
    , ui(new Ui::ReactionsDialog)
    , reactionsTableModel_(new ReactionsTableModel(this, abstractSimulationBuilder))
{
    ui->setupUi(this);

    connect(ui->okPushButton, &QPushButton::clicked,
            utility::safeSlot(this,
                              [this]()
                              {
                                  reactionsTableModel_->commitChanges();
                                  hide();
                              }));
    connect(ui->cancelPushButton, &QPushButton::clicked, this, &ReactionsDialog::cancel);

    connect(ui->addCombinationReactionPushButton, &QPushButton::clicked,
            utility::safeSlot(this, [this]() { reactionsTableModel_->addRow(cell::Reaction::Type::Combination); }));
    connect(ui->addDecompositionReactionPushButton, &QPushButton::clicked,
            utility::safeSlot(this, [this]() { reactionsTableModel_->addRow(cell::Reaction::Type::Decomposition); }));
    connect(ui->addExchangeReactionPushButton, &QPushButton::clicked,
            utility::safeSlot(this, [this]() { reactionsTableModel_->addRow(cell::Reaction::Type::Exchange); }));
    connect(ui->addTransformationReactionPushButton, &QPushButton::clicked,
            utility::safeSlot(this, [this]() { reactionsTableModel_->addRow(cell::Reaction::Type::Transformation); }));

    connect(ui->clearReactionsPushButton, &QPushButton::clicked, reactionsTableModel_, &ReactionsTableModel::clearRows);

    insertDeleteButtonIntoView(this, reactionsTableModel_, ui->reactionsTableView, 8);
    insertDiscTypeComboboxIntoView(ui->reactionsTableView, abstractSimulationBuilder, 0, 2, 4, 6);
    insertProbabilitySpinBoxIntoView(ui->reactionsTableView, 7);

    ui->reactionsTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->reactionsTableView->setModel(reactionsTableModel_);
}

void ReactionsDialog::closeEvent(QCloseEvent*)
{
    reactionsTableModel_->discardChanges();
}

void ReactionsDialog::cancel()
{
    reactionsTableModel_->discardChanges();
    hide();
}
