#include "ReactionsDialog.hpp"
#include "GlobalSettings.hpp"
#include "Utility.hpp"
#include "ui_ReactionsDialog.h"

#include <QCloseEvent>
#include <QComboBox>
#include <QMessageBox>
#include <QPushButton>

#include <unordered_set>

ReactionsDialog::ReactionsDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::ReactionsDialog)
    , reactionsModel_(new QStandardItemModel(this))
{
    // TODO Create base class for the 2 similar dialogs (DiscTypeDistributionDialog and this one)

    ui->setupUi(this);

    Utility::setModelHeaderData(reactionsModel_, {"A", "+", "B", "->", "C", "+", "D", "Probability [%]", "Delete"});
    ui->reactionsTableView->setModel(reactionsModel_);

    connect(ui->okPushButton, &QPushButton::clicked, this, &ReactionsDialog::onOK);
    connect(ui->cancelPushButton, &QPushButton::clicked, this, &ReactionsDialog::onCancel);

    connect(ui->addCombinationReactionPushButton, &QPushButton::clicked, this,
            &ReactionsDialog::onAddCombinationReaction);
    connect(ui->addDecompositionReactionPushButton, &QPushButton::clicked, this,
            &ReactionsDialog::onAddDecompositionReaction);
    connect(ui->addExchangeReactionPushButton, &QPushButton::clicked, this, &ReactionsDialog::onAddExchangeReaction);

    connect(ui->clearReactionsPushButton, &QPushButton::clicked, this, &ReactionsDialog::onClearReactions);
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

    resetTableViewToSettings();
}

void ReactionsDialog::onOK()
{
    const auto& reactions = convertInputsToReactions();

    try
    {
        GlobalSettings::get().clearReactions();
        for (const auto& reaction : reactions)
            GlobalSettings::get().addReaction(reaction);

        emit reactionsChanged();
        hide();
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, "Error", e.what());
    }
}

void ReactionsDialog::onCancel()
{
    resetTableViewToSettings();
    hide();
}

void ReactionsDialog::onAddCombinationReaction()
{
    addRowFromReaction(
        {getEduct1() = getDefaultDiscType(), .educt2_ = getDefaultDiscType(), .product1_ = getDefaultDiscType()});
}

void ReactionsDialog::onAddDecompositionReaction()
{
    addRowFromReaction(
        {getEduct1() = getDefaultDiscType(), .product1_ = getDefaultDiscType(), .product2_ = getDefaultDiscType()});
}

void ReactionsDialog::onAddExchangeReaction()
{
    addRowFromReaction({getEduct1() = getDefaultDiscType(), .educt2_ = getDefaultDiscType(),
                        .product1_ = getDefaultDiscType(), .product2_ = getDefaultDiscType()});
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

void ReactionsDialog::addRowFromReaction(const Reaction& reaction)
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

    std::vector<DiscType> reactionParts = {reactiongetEduct1(), reaction.educt2_, reaction.product1_,
                                           reaction.product2_};
    for (int i = 0; i < reactionParts.size(); ++i)
    {
        if (reactionParts[i].name_.empty())
            continue;

        Utility::addComboBoxToLastRow(discTypeNames, QString::fromStdString(reactionParts[i].name_), reactionsModel_,
                                      ui->reactionsTableView, i * 2);
    }

    Utility::addSpinBoxToLastRow<QDoubleSpinBox>(reaction.probability_, 0, 100, ui->reactionsTableView, reactionsModel_,
                                                 7);

    QPushButton* deleteButton = new QPushButton("Delete");
    connect(deleteButton, &QPushButton::clicked, this, &ReactionsDialog::onDeleteReaction);
    ui->reactionsTableView->setIndexWidget(reactionsModel_->index(reactionsModel_->rowCount() - 1, 8), deleteButton);
}

void ReactionsDialog::resetTableViewToSettings()
{
    onClearReactions();

    const auto& settings = GlobalSettings::getSettings();

    // Reactions for {A, B} are duplicated in the maps with {B, A} for easier lookup
    // Gotta ignore those duplicates here
    std::unordered_set<Reaction, ReactionHash> reactionSet;

    auto collectReactions = [&](const auto& reactionsMap)
    {
        for (const auto& [key, reactions] : reactionsMap)
        {
            for (const auto& reaction : reactions)
                reactionSet.insert(reaction);
        }
    };

    collectReactions(settings.decompositionReactions_);
    collectReactions(settings.combinationReactions_);
    collectReactions(settings.exchangeReactions_);

    for (const auto& reaction : reactionSet)
        addRowFromReaction(reaction);
}

std::vector<Reaction> ReactionsDialog::convertInputsToReactions() const
{
    std::vector<Reaction> reactions;

    for (int row = 0; row < reactionsModel_->rowCount(); ++row)
    {
        Reaction reaction;
        std::vector<DiscType*> reactionParts{&reactiongetEduct1(), &reaction.educt2_, &reaction.product1_,
                                             &reaction.product2_};

        for (int i = 0; i < 4; ++i)
        {
            // Reminder: "A", "+", "B", "->", "C", "+", "D", "Probability [%]", "Delete"
            int widgetColumn = i * 2;
            QWidget* widget = ui->reactionsTableView->indexWidget(reactionsModel_->index(row, widgetColumn));

            if (!widget)
                continue;

            QComboBox* comboBox = qobject_cast<QComboBox*>(widget);
            *reactionParts[i] = GlobalSettings::getDiscTypeByName(comboBox->currentText().toStdString());
        }

        QDoubleSpinBox* spinBox =
            qobject_cast<QDoubleSpinBox*>(ui->reactionsTableView->indexWidget(reactionsModel_->index(row, 7)));
        reaction.probability_ = spinBox->value();

        reactions.push_back(reaction);
    }

    return reactions;
}

const DiscType& ReactionsDialog::getDefaultDiscType() const
{
    return GlobalSettings::getSettings().discTypeDistribution_.begin()->first;
}
