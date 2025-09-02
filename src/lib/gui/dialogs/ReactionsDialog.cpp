#include "dialogs/ReactionsDialog.hpp"
#include "core/SafeCast.hpp"
#include "core/Utility.hpp"
#include "delegates/ButtonDelegate.hpp"
#include "delegates/ComboBoxDelegate.hpp"
#include "delegates/SpinBoxDelegate.hpp"
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

    connect(ui->clearReactionsPushButton, &QPushButton::clicked, reactionsTableModel_, &ReactionsTableModel::clearRows);

    using SpinBoxDelegate = SpinBoxDelegate<QDoubleSpinBox>;

    auto* deleteButtonDelegate = new ButtonDelegate(this, "Delete");
    auto* discTypeComboBoxDelegate = new DiscTypeComboBoxDelegate(this);
    auto* probabilitySpinBoxDelegate = new SpinBoxDelegate(this);

    connect(deleteButtonDelegate, &ButtonDelegate::buttonClicked, reactionsTableModel_,
            &ReactionsTableModel::removeRow);
    connect(probabilitySpinBoxDelegate, &SpinBoxDelegate::editorCreated,
            [](QWidget* spinBox)
            {
                safeCast<QDoubleSpinBox*>(spinBox)->setRange(0.0, 1.0);
                safeCast<QDoubleSpinBox*>(spinBox)->setSingleStep(0.001);
                safeCast<QDoubleSpinBox*>(spinBox)->setDecimals(3);
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

void ReactionsDialog::closeEvent(QCloseEvent*)
{
    reactionsTableModel_->loadSettings();
}

void ReactionsDialog::cancel()
{
    reactionsTableModel_->loadSettings();
    hide();
}
