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
    // TODO Create base class for the 2 similar dialogs (DiscTypesDialog and this one)

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
    const auto& decompositionReactions = convertInputsToDecompositionReactions();

    try
    {
        GlobalSettings::get().setCombinationReactions(combinationReactions);
        GlobalSettings::get().setDecompositionReactions(decompositionReactions);
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

    for (const auto& [educts, product] : GlobalSettings::getSettings().reactionTable_.getCombinationReactionsTable())
        addRowFromCombinationReaction(educts, product);

    for (const auto& [educt, products] : GlobalSettings::getSettings().reactionTable_.getDecompositionReactionsTable())
        addRowFromDecompositionReaction(educt, products);
}

std::vector<CombinationReaction> ReactionsDialog::convertInputsToCombinationReactions() const
{
    std::vector<CombinationReaction> combinationReactions;

    for (int row = 0; row < reactionsModel_->rowCount(); ++row)
    {
        if (getRowReactionType(row) != ReactionType::Combination)
            continue;

        QComboBox* aComboBox =
            qobject_cast<QComboBox*>(ui->reactionsTableView->indexWidget(reactionsModel_->index(row, 0)));
        QComboBox* bComboBox =
            qobject_cast<QComboBox*>(ui->reactionsTableView->indexWidget(reactionsModel_->index(row, 2)));
        QComboBox* cComboBox =
            qobject_cast<QComboBox*>(ui->reactionsTableView->indexWidget(reactionsModel_->index(row, 4)));
        QDoubleSpinBox* spinBox =
            qobject_cast<QDoubleSpinBox*>(ui->reactionsTableView->indexWidget(reactionsModel_->index(row, 7)));

        combinationReactions.push_back(
            CombinationReaction{.educt1 = GlobalSettings::getDiscTypeByName(aComboBox->currentText().toStdString()),
                                .educt2 = GlobalSettings::getDiscTypeByName(bComboBox->currentText().toStdString()),
                                .product = GlobalSettings::getDiscTypeByName(cComboBox->currentText().toStdString()),
                                .probability = static_cast<float>(spinBox->value())});
    }

    return combinationReactions;
}

std::vector<DecompositionReaction> ReactionsDialog::convertInputsToDecompositionReactions() const
{
    std::vector<DecompositionReaction> decompositionReactions;

    for (int row = 0; row < reactionsModel_->rowCount(); ++row)
    {
        if (getRowReactionType(row) != ReactionType::Decomposition)
            continue;

        QComboBox* aComboBox =
            qobject_cast<QComboBox*>(ui->reactionsTableView->indexWidget(reactionsModel_->index(row, 0)));
        QComboBox* cComboBox =
            qobject_cast<QComboBox*>(ui->reactionsTableView->indexWidget(reactionsModel_->index(row, 4)));
        QComboBox* dComboBox =
            qobject_cast<QComboBox*>(ui->reactionsTableView->indexWidget(reactionsModel_->index(row, 6)));
        QDoubleSpinBox* spinBox =
            qobject_cast<QDoubleSpinBox*>(ui->reactionsTableView->indexWidget(reactionsModel_->index(row, 7)));

        decompositionReactions.push_back(
            {.educt = GlobalSettings::getDiscTypeByName(aComboBox->currentText().toStdString()),
             .product1 = GlobalSettings::getDiscTypeByName(cComboBox->currentText().toStdString()),
             .product2 = GlobalSettings::getDiscTypeByName(dComboBox->currentText().toStdString()),
             .probability = static_cast<float>(spinBox->value())});
    }

    return decompositionReactions;
}

ReactionType ReactionsDialog::getRowReactionType(int index) const
{
    // If we have a widget in column 3 it has to be a combination reaction, decomposition reactions don't have that
    // column
    QWidget* widget = ui->reactionsTableView->indexWidget(reactionsModel_->index(index, 2));

    if (widget)
        return ReactionType::Combination;

    return ReactionType::Decomposition;
}
