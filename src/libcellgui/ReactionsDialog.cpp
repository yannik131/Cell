#include "ReactionsDialog.hpp"
#include "ButtonDelegate.hpp"
#include "ComboBoxDelegate.hpp"
#include "GlobalSimulationSettings.hpp"
#include "SpinBoxDelegate.hpp"
#include "Utility.hpp"
#include "ui_ReactionsDialog.h"

#include <QCloseEvent>
#include <QMessageBox>

ReactionsDialog::ReactionsDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::ReactionsDialog)
    , reactionsTableModel_(new ReactionsTableModel(this))
{
    ui->setupUi(this);

    connect(ui->okPushButton, &QPushButton::clicked,
            [this]()
            {
                try
                {
                    reactionsTableModel_->saveSettings();
                    hide();
                }
                catch (const std::exception& error)
                {
                    QMessageBox::critical(this, "Error", error.what());
                }
            });
    connect(ui->cancelPushButton, &QPushButton::clicked, this, &ReactionsDialog::cancel);

    connect(ui->addCombinationReactionPushButton, &QPushButton::clicked,
            [this]() { requestEmptyRowFromModel(Reaction::Type::Combination); });
    connect(ui->addDecompositionReactionPushButton, &QPushButton::clicked,
            [this]() { requestEmptyRowFromModel(Reaction::Type::Decomposition); });
    connect(ui->addExchangeReactionPushButton, &QPushButton::clicked,
            [this]() { requestEmptyRowFromModel(Reaction::Type::Exchange); });

    connect(ui->clearReactionsPushButton, &QPushButton::clicked, reactionsTableModel_, &ReactionsTableModel::clearRows);
    connect(&GlobalSimulationSettings::get(), &GlobalSimulationSettings::discTypeDistributionChanged, this,
            &ReactionsDialog::updateComboBoxDelegateItems);

    ButtonDelegate* buttonDelegate = new ButtonDelegate(this);
    comboBoxDelegate_ = new ComboBoxDelegate(this);
    SpinBoxDelegate* spinBoxDelegate = new SpinBoxDelegate(this);

    ui->reactionsTableView->setItemDelegateForColumn(0, comboBoxDelegate_);
    ui->reactionsTableView->setItemDelegateForColumn(2, comboBoxDelegate_);
    ui->reactionsTableView->setItemDelegateForColumn(4, comboBoxDelegate_);
    ui->reactionsTableView->setItemDelegateForColumn(6, comboBoxDelegate_);
    ui->reactionsTableView->setItemDelegateForColumn(7, spinBoxDelegate);
    ui->reactionsTableView->setItemDelegateForColumn(8, buttonDelegate);
}

void ReactionsDialog::closeEvent(QCloseEvent* event)
{
    cancel();
    event->ignore();
}

void ReactionsDialog::requestEmptyRowFromModel(const Reaction::Type& type)
{
    try
    {
        reactionsTableModel_->addEmptyRow(type);
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, "Error", "Can't add row: " + QString(e.what()));
    }
}

void ReactionsDialog::cancel()
{
    reactionsTableModel_->loadSettings();
    hide();
}

void ReactionsDialog::updateComboBoxDelegateItems()
{
    QStringList discTypeNames;
    for (const auto& [discType, _] : GlobalSimulationSettings::getSettings().discTypeDistribution_)
        discTypeNames.push_back(QString::fromStdString(discType.getName()));

    comboBoxDelegate_->setAvailableItems(std::move(discTypeNames));
}
