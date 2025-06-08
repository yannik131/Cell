#include "ReactionsDialog.hpp"
#include "ButtonDelegate.hpp"
#include "ComboBoxDelegate.hpp"
#include "GlobalSettings.hpp"
#include "GlobalSettingsFunctor.hpp"
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
    connect(ui->addTransformationReactionPushButton, &QPushButton::clicked,
            [this]() { requestEmptyRowFromModel(Reaction::Type::Transformation); });

    connect(ui->clearReactionsPushButton, &QPushButton::clicked, reactionsTableModel_, &ReactionsTableModel::clearRows);

    using SpinBoxDelegate = SpinBoxDelegate<QDoubleSpinBox>;

    auto* deleteButtonDelegate = new ButtonDelegate(this, "Delete");
    ComboBoxDelegate* discTypeComboBoxDelegate = new DiscTypeComboBoxDelegate(this);
    auto* probabilitySpinBoxDelegate = new SpinBoxDelegate(this);

    connect(deleteButtonDelegate, &ButtonDelegate::buttonClicked, reactionsTableModel_,
            &ReactionsTableModel::removeRow);
    connect(probabilitySpinBoxDelegate, &SpinBoxDelegate::editorCreated,
            [](QWidget* spinBox)
            {
                qobject_cast<QDoubleSpinBox*>(spinBox)->setRange(0.0, 1.0);
                qobject_cast<QDoubleSpinBox*>(spinBox)->setSingleStep(0.001);
                qobject_cast<QDoubleSpinBox*>(spinBox)->setDecimals(3);
            });

    ui->reactionsTableView->setItemDelegateForColumn(0, discTypeComboBoxDelegate);
    ui->reactionsTableView->setItemDelegateForColumn(2, discTypeComboBoxDelegate);
    ui->reactionsTableView->setItemDelegateForColumn(4, discTypeComboBoxDelegate);
    ui->reactionsTableView->setItemDelegateForColumn(6, discTypeComboBoxDelegate);
    ui->reactionsTableView->setItemDelegateForColumn(7, probabilitySpinBoxDelegate);
    ui->reactionsTableView->setItemDelegateForColumn(8, deleteButtonDelegate);
    ui->reactionsTableView->setEditTriggers(QAbstractItemView::EditTrigger::CurrentChanged |
                                            QAbstractItemView::EditTrigger::SelectedClicked);

    ui->reactionsTableView->setModel(reactionsTableModel_);
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
