#include "DiscTypeDistributionDialog.hpp"
#include "ButtonDelegate.hpp"
#include "ComboBoxDelegate.hpp"
#include "SpinBoxDelegate.hpp"
#include "ui_DiscTypeDistributionDialog.h"

#include <QCloseEvent>

DiscTypeDistributionDialog::DiscTypeDistributionDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::DiscTypeDistributionDialog)
{
    ui->setupUi(this);

    ButtonDelegate* buttonDelegate = new ButtonDelegate(this);
    ComboBoxDelegate* comboBoxDelegate = new ComboBoxDelegate(this);
    SpinBoxDelegate* spinBoxDelegate = new SpinBoxDelegate(this);

    ui->discDistributionTableView->setItemDelegateForColumn(1, spinBoxDelegate);
    ui->discDistributionTableView->setItemDelegateForColumn(2, spinBoxDelegate);
    ui->discDistributionTableView->setItemDelegateForColumn(3, comboBoxDelegate);
    ui->discDistributionTableView->setItemDelegateForColumn(4, spinBoxDelegate);
    ui->discDistributionTableView->setItemDelegateForColumn(5, buttonDelegate);
}

void DiscTypeDistributionDialog::closeEvent(QCloseEvent* event)
{
    emit dialogClosed();
}

void DiscTypeDistributionDialog::setModel(DiscTypeDistributionTableModel* discTypeDistributionTableModel)
{
    connect(ui->okPushButton, &QPushButton::clicked, discTypeDistributionTableModel,
            [this, = ]()
            {
                try
                {
                    discTypeDistributionTableModel->saveSettings();
                    hide();
                }
                catch (const std::runtime_error& error)
                {
                    QMessageBox::critical(this, "Error", "Couldn't save disc distribution: " + QString(error.what()));
                }
            });
    connect(ui->cancelPushButton, &QPushButton::clicked, discTypeDistributionTableModel,
            &DiscTypeDistributionTableModel::loadSettings);
    connect(ui->addTypePushButton, &QPushButton::clicked, discTypeDistributionTableModel,
            [=]()
            {
                discTypeDistributionTableModel->addRowFromDiscType(
                    DiscType{.color_ = ColorNameMapping.keys().front(), .mass_ = 0, .name_ = "", .radius_ = 0});
            });
    connect(ui->clearTypesPushButton, &QPushButton::clicked, discTypeDistributionTableModel,
            &DiscTypeDistributionTableModel::clearRows);
    connect(this, &DiscTypeDistributionDialog::dialogClosed, discTypeDistributionTableModel,
            &DiscTypeDistributionTableModel::loadSettings);
}
