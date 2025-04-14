#include "DiscTypeDistributionDialog.hpp"
#include "ButtonDelegate.hpp"
#include "ColorMapping.hpp"
#include "ComboBoxDelegate.hpp"
#include "GlobalSettingsFunctor.hpp"
#include "SpinBoxDelegate.hpp"
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
    connect(ui->cancelPushButton, &QPushButton::clicked, discTypeDistributionTableModel_,
            &DiscTypeDistributionTableModel::loadSettings);
    connect(ui->addTypePushButton, &QPushButton::clicked, discTypeDistributionTableModel_,
            [this]()
            {
                discTypeDistributionTableModel_->addRowFromDiscType(
                    DiscType{"NewType", getSupportedDiscColors().front(), 0, 0});
            });
    connect(ui->clearTypesPushButton, &QPushButton::clicked, discTypeDistributionTableModel_,
            &DiscTypeDistributionTableModel::clearRows);
    connect(this, &DiscTypeDistributionDialog::dialogClosed, discTypeDistributionTableModel_,
            &DiscTypeDistributionTableModel::loadSettings);
    connect(&GlobalSettingsFunctor::get(), &GlobalSettingsFunctor::discTypeDistributionChanged,
            discTypeDistributionTableModel_, &DiscTypeDistributionTableModel::loadSettings);

    ButtonDelegate* buttonDelegate = new ButtonDelegate(this);
    ComboBoxDelegate* comboBoxDelegate = new ComboBoxDelegate(this);
    SpinBoxDelegate* spinBoxDelegate = new SpinBoxDelegate(this);

    comboBoxDelegate->setAvailableItems(getSupportedDiscColorNames());

    ui->discTypeDistributionTableView->setItemDelegateForColumn(1, spinBoxDelegate);
    ui->discTypeDistributionTableView->setItemDelegateForColumn(2, spinBoxDelegate);
    ui->discTypeDistributionTableView->setItemDelegateForColumn(3, comboBoxDelegate);
    ui->discTypeDistributionTableView->setItemDelegateForColumn(4, spinBoxDelegate);
    ui->discTypeDistributionTableView->setItemDelegateForColumn(5, buttonDelegate);
}

void DiscTypeDistributionDialog::closeEvent(QCloseEvent*)
{
    emit dialogClosed();
}
