#include "ReactionsDialog.hpp"
#include "GlobalSettings.hpp"
#include "Utility.hpp"
#include "ui_ReactionsDialog.h"

#include <QCloseEvent>
#include <QComboBox>
#include <QMessageBox>
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
    connect(ui->addCombinationReactionPushButton, &QPushButton::clicked, this,
            &ReactionsDialog::onAddCombinationReaction);
    connect(ui->addDecompositionReactionPushButton, &QPushButton::clicked, this,
            &ReactionsDialog::onAddDecompositionReaction);
    connect(ui->clearReactionsPushButton, &QPushButton::clicked, this, &ReactionsDialog::onClearReactions);

    resetTableViewToSettings();
}

void ReactionsDialog::closeEvent(QCloseEvent* event)
{
    onCancel();
    event->ignore();
}

void ReactionsDialog::showEvent(QShowEvent* event)
{
    if (GlobalSettings::getSettings().discTypeDistribution_.empty())
    {
        QMessageBox::information(this, "Information",
                                 "No disc types available for reactions.\nCreate some disc types first.");
        event->ignore();
    }
}

void ReactionsDialog::onOK()
{
    const auto& combinationReactions = convertInputsToCombinationReactions();
    const auto& decompositionReactions = convertInputsToCombinationReactions();

    GlobalSettings::get().set
}

void ReactionsDialog::onCancel()
{
    resetTableViewToSettings();
    hide();
}

void ReactionsDialog::onAddCombinationReaction()
{
    const QString& firstDiscTypeName =
        QString::fromStdString(GlobalSettings::getSettings().discTypeDistribution_.begin()->first.name_);
    addReactionRow({firstDiscTypeName, firstDiscTypeName, firstDiscTypeName, ""}, 0);
}

void ReactionsDialog::onAddDecompositionReaction()
{
    const QString& firstDiscTypeName =
        QString::fromStdString(GlobalSettings::getSettings().discTypeDistribution_.begin()->first.name_);
    addReactionRow({firstDiscTypeName, "", firstDiscTypeName, firstDiscTypeName}, 0);
}

void ReactionsDialog::onClearReactions()
{
    reactionsModel_->removeRows(0, reactionsModel_->rowCount());
}

void ReactionsDialog::onDeleteReaction()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (!button)
        return;

    QModelIndex index = ui->reactionsTableView->indexAt(button->pos());

    reactionsModel_->removeRow(index.row());
}

void ReactionsDialog::addRowFromCombinationReaction(const std::pair<DiscType, DiscType>& educts,
                                                    const std::vector<std::pair<DiscType, float>>& products)
{
    for (const auto& [discType, probability] : products)
    {
        std::array<QString, 4> selectedDiscTypes = {QString::fromStdString(educts.first.name_),
                                                    QString::fromStdString(educts.second.name_),
                                                    QString::fromStdString(discType.name_), ""};

        addReactionRow(selectedDiscTypes, probability);
    }
}

void ReactionsDialog::addRowFromDecompositionReaction(
    const DiscType& educt, const std::vector<std::pair<std::pair<DiscType, DiscType>, float>>& products)
{
    for (const auto& [product, probability] : products)
    {
        std::array<QString, 4> selectedDiscTypes = {QString::fromStdString(educt.name_), "",
                                                    QString::fromStdString(product.first.name_),
                                                    QString::fromStdString(product.second.name_)};

        addReactionRow(selectedDiscTypes, probability);
    }
}

void ReactionsDialog::addReactionRow(std::array<QString, 4> selectedDiscTypes, float probability)
{
    QStringList discTypeNames;
    for (const auto& [discType, frequency] : GlobalSettings::getSettings().discTypeDistribution_)
        discTypeNames.push_back(QString::fromStdString(discType.name_));

    QList<QStandardItem*> items;
    for (int i = 0; i < reactionsModel_->columnCount(); ++i)
    {
        QStandardItem* item = new QStandardItem();
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        items.append(item);
    }

    reactionsModel_->appendRow(items);

    // Reminder: "A", "+", "B", "->", "C", "+", "D", "Probability [%]", "Delete"

    for (int i = 0; i < selectedDiscTypes.size(); ++i)
    {
        const auto& selectedDiscType = selectedDiscTypes[i];
        if (selectedDiscType.isEmpty())
            continue;

        Utility::addComboBoxToLastRow(discTypeNames, selectedDiscType, reactionsModel_, ui->reactionsTableView, i * 2);
    }

    Utility::addSpinBoxToLastRow<QDoubleSpinBox>(probability, 0, 100, ui->reactionsTableView, reactionsModel_, 7);

    QPushButton* deleteButton = new QPushButton("Delete");
    connect(deleteButton, &QPushButton::clicked, this, &ReactionsDialog::onDeleteReaction);
    ui->reactionsTableView->setIndexWidget(reactionsModel_->index(reactionsModel_->rowCount() - 1, 8), deleteButton);
}

void ReactionsDialog::resetTableViewToSettings()
{
    onClearReactions();

    for (const auto& [educts, product] : GlobalSettings::getSettings().combinationReactionTable_)
        addRowFromCombinationReaction(educts, product);

    for (const auto& [educt, products] : GlobalSettings::getSettings().decompositionReactionTable_)
        addRowFromDecompositionReaction(educt, products);
}

std::map<std::pair<DiscType, DiscType>, std::vector<std::pair<DiscType, float>>>
ReactionsDialog::convertInputsToCombinationReactions() const
{
    return std::map<std::pair<DiscType, DiscType>, std::vector<std::pair<DiscType, float>>>();
}

std::map<DiscType, std::vector<std::pair<std::pair<DiscType, DiscType>, float>>>
ReactionsDialog::convertInputsToDecompositionReactions() const
{
    return std::map<DiscType, std::vector<std::pair<std::pair<DiscType, DiscType>, float>>>();
}
