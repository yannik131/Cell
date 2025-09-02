#include "dialogs/DiscTypeDistributionDialog.hpp"
#include "cell/Settings.hpp"
#include "core/ColorMapping.hpp"
#include "core/SafeCast.hpp"
#include "delegates/ButtonDelegate.hpp"
#include "delegates/ComboBoxDelegate.hpp"
#include "delegates/SpinBoxDelegate.hpp"
#include "ui_DiscTypeDistributionDialog.h"

#include <QCloseEvent>
#include <QMessageBox>

DiscTypeDistributionDialog::DiscTypeDistributionDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::DiscTypeDistributionDialog)
    , discTypeDistributionTableModel_(new DiscTypeDistributionTableModel(this))
{
    ui->setupUi(this);

    connect(ui->okPushButton, &QPushButton::clicked,
            [this]()
            {
                try
                {
                    discTypeDistributionTableModel_->saveSettings();
                    hide();
                }
                catch (const std::runtime_error& error)
                {
                    QMessageBox::critical(this, "Error", "Couldn't save disc distribution: " + QString(error.what()));
                }
            });
    connect(ui->cancelPushButton, &QPushButton::clicked, this, &DiscTypeDistributionDialog::cancel);
    connect(ui->addTypePushButton, &QPushButton::clicked, discTypeDistributionTableModel_,
            &DiscTypeDistributionTableModel::addEmptyRow);
    connect(ui->clearTypesPushButton, &QPushButton::clicked, discTypeDistributionTableModel_,
            &DiscTypeDistributionTableModel::clearRows);

    auto* colorComboBoxDelegate = new ComboBoxDelegate(this);
    auto* radiusSpinBoxDelegate = new SpinBoxDelegate<QDoubleSpinBox>(this);
    auto* massSpinBoxDelegate = new SpinBoxDelegate<QDoubleSpinBox>(this);
    auto* frequencySpinBoxDelegate = new SpinBoxDelegate<QSpinBox>(this);
    auto* deleteButtonDelegate = new ButtonDelegate(this, "Delete");

    connect(colorComboBoxDelegate, &ComboBoxDelegate::editorCreated,
            [](QComboBox* comboBox) { comboBox->addItems(getSupportedDiscColorNames()); });
    connect(radiusSpinBoxDelegate, &SpinBoxDelegate<QDoubleSpinBox>::editorCreated,
            [](QWidget* spinBox)
            {
                safeCast<QDoubleSpinBox*>(spinBox)->setRange(cell::DiscTypeLimits::MinRadius,
                                                             cell::DiscTypeLimits::MaxRadius);
            });
    connect(massSpinBoxDelegate, &SpinBoxDelegate<QDoubleSpinBox>::editorCreated,
            [](QWidget* spinBox)
            {
                safeCast<QDoubleSpinBox*>(spinBox)->setRange(cell::DiscTypeLimits::MinMass,
                                                             cell::DiscTypeLimits::MaxMass);
            });
    connect(frequencySpinBoxDelegate, &SpinBoxDelegate<QSpinBox>::editorCreated,
            [](QWidget* spinBox) { safeCast<QSpinBox*>(spinBox)->setRange(0, 100); });
    connect(deleteButtonDelegate, &ButtonDelegate::buttonClicked, discTypeDistributionTableModel_,
            &DiscTypeDistributionTableModel::removeRow);

    ui->discTypeDistributionTableView->setItemDelegateForColumn(1, radiusSpinBoxDelegate);
    ui->discTypeDistributionTableView->setItemDelegateForColumn(2, massSpinBoxDelegate);
    ui->discTypeDistributionTableView->setItemDelegateForColumn(3, colorComboBoxDelegate);
    ui->discTypeDistributionTableView->setItemDelegateForColumn(4, frequencySpinBoxDelegate);
    ui->discTypeDistributionTableView->setItemDelegateForColumn(5, deleteButtonDelegate);
    ui->discTypeDistributionTableView->setEditTriggers(QAbstractItemView::EditTrigger::CurrentChanged |
                                                       QAbstractItemView::EditTrigger::SelectedClicked);

    ui->discTypeDistributionTableView->setModel(discTypeDistributionTableModel_);
}

void DiscTypeDistributionDialog::closeEvent(QCloseEvent*)
{
    discTypeDistributionTableModel_->loadSettings();
}

void DiscTypeDistributionDialog::cancel()
{
    discTypeDistributionTableModel_->loadSettings();
    hide();
}
