#include "ReactionsDialog.hpp"
#include "GlobalSettings.hpp"
#include "Utility.hpp"
#include "ui_ReactionsDialog.h"

#include <QComboBox>
#include <QPushButton>

ReactionsDialog::ReactionsDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::ReactionsDialog)
    , reactionsModel_(new QStandardItemModel(this))
{
    ui->setupUi(this);

    Utility::setModelHeaderData(reactionsModel_, {"A", "+", "B", "->", "C", "+", "D", "Probability [%]", "Delete"});
    ui->reactionsTableView->setModel(reactionsModel_);

    connect(ui->okPushButton, &QPushButton::clicked, this, &ReactionsDialog::onOK);
    connect(ui->cancelPushButton, &QPushButton::clicked, this, &ReactionsDialog::onCancel);
    connect(ui->addReactionPushButton, &QPushButton::clicked, this, &ReactionsDialog::onAddReaction);
    connect(ui->clearReactionsPushButton, &QPushButton::clicked, this, &ReactionsDialog::onClearReactions);

    for (const auto& [educts, product] : GlobalSettings::getSettings().combinationReactionTable_)
        addTableViewRowFromCombinationReaction(educts, product);

    for (const auto& [educt, products] : GlobalSettings::getSettings().decompositionReactionTable_)
        addTableViewRowFromDecompositionReaction(educt, products);
}

void ReactionsDialog::closeEvent(QCloseEvent* event)
{
}

void ReactionsDialog::onOK()
{
}

void ReactionsDialog::onCancel()
{
}

void ReactionsDialog::onAddReaction()
{
}

void ReactionsDialog::onClearReactions()
{
}

void ReactionsDialog::onDeleteReaction()
{
}

void ReactionsDialog::addTableViewRowFromCombinationReaction(const std::pair<DiscType, DiscType>& educts,
                                                             const std::vector<std::pair<DiscType, float>>& products)
{
    using Utility::addComboBoxToLastRow;
    using Utility::addSpinBoxToLastRow;

    QStringList discTypeNames;
    for (const auto& [discType, frequency] : GlobalSettings::getSettings().discTypeDistribution_)
        discTypeNames.push_back(QString::fromStdString(discType.name_));

    // Reminder: "A", "+", "B", "->", "C", "+", "D", "Probability [%]", "Delete"

    for (const auto& [discType, probability] : products)
    {
        QList<QStandardItem*> items;
        for (int i = 0; i < reactionsModel_->columnCount(); ++i)
        {
            QStandardItem* item = new QStandardItem();
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            items.append(item);
        }

        reactionsModel_->appendRow(items);

        addComboBoxToLastRow(discTypeNames, QString::fromStdString(educts.first.name_), reactionsModel_,
                             ui->reactionsTableView, 0);
        addComboBoxToLastRow(discTypeNames, QString::fromStdString(educts.second.name_), reactionsModel_,
                             ui->reactionsTableView, 2);

        addComboBoxToLastRow(discTypeNames, QString::fromStdString(discType.name_), reactionsModel_,
                             ui->reactionsTableView, 4);

        addSpinBoxToLastRow<QDoubleSpinBox>(probability, 0, 100, ui->reactionsTableView, reactionsModel_, 7);

        QPushButton* deleteButton = new QPushButton("Delete");
        connect(deleteButton, &QPushButton::clicked, this, &ReactionsDialog::onDeleteReaction);
        ui->reactionsTableView->setIndexWidget(reactionsModel_->index(reactionsModel_->rowCount() - 1, 8),
                                               deleteButton);
    }
}

void ReactionsDialog::addTableViewRowFromDecompositionReaction(
    const DiscType& educt, const std::vector<std::pair<std::pair<DiscType, DiscType>, float>>& products)
{
    using Utility::addComboBoxToLastRow;
    using Utility::addSpinBoxToLastRow;

    QStringList discTypeNames;
    for (const auto& [discType, frequency] : GlobalSettings::getSettings().discTypeDistribution_)
        discTypeNames.push_back(QString::fromStdString(discType.name_));

    // Reminder: "A", "+", "B", "->", "C", "+", "D", "Probability [%]", "Delete"

    for (const auto& [product, probability] : products)
    {
        QList<QStandardItem*> items;
        for (int i = 0; i < reactionsModel_->columnCount(); ++i)
        {
            QStandardItem* item = new QStandardItem();
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            items.append(item);
        }

        reactionsModel_->appendRow(items);

        addComboBoxToLastRow(discTypeNames, QString::fromStdString(educt.name_), reactionsModel_,
                             ui->reactionsTableView, 0);

        addComboBoxToLastRow(discTypeNames, QString::fromStdString(product.first.name_), reactionsModel_,
                             ui->reactionsTableView, 4);
        addComboBoxToLastRow(discTypeNames, QString::fromStdString(product.second.name_), reactionsModel_,
                             ui->reactionsTableView, 6);

        addSpinBoxToLastRow<QDoubleSpinBox>(probability, 0, 100, ui->reactionsTableView, reactionsModel_, 7);

        QPushButton* deleteButton = new QPushButton("Delete");
        connect(deleteButton, &QPushButton::clicked, this, &ReactionsDialog::onDeleteReaction);
        ui->reactionsTableView->setIndexWidget(reactionsModel_->index(reactionsModel_->rowCount() - 1, 8),
                                               deleteButton);
    }
}
