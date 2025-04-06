#include "DiscTypeDistributionDialog.hpp"
#include "ColorMapping.hpp"
#include "DiscType.hpp"
#include "GlobalSettings.hpp"
#include "Utility.hpp"
#include "ui_DiscTypeDistributionDialog.h"

#include <QCloseEvent>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <glog/logging.h>

#include <map>

DiscTypeDistributionDialog::DiscTypeDistributionDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::DiscTypeDistributionDialog)
    , discTypesModel_(new QStandardItemModel(this))
{
    ui->setupUi(this);

    Utility::setModelHeaderData(discTypesModel_,
                                {"Disc type", "Radius [px]", "Mass", "Color", "Frequency [%]", "Delete"});

    ui->discDistributionTableView->setModel(discTypesModel_);

    connect(ui->okPushButton, &QPushButton::clicked, this, &DiscTypeDistributionDialog::onOK);
    connect(ui->cancelPushButton, &QPushButton::clicked, this, &DiscTypeDistributionDialog::onCancel);
    connect(ui->addTypePushButton, &QPushButton::clicked, this, &DiscTypeDistributionDialog::onAddType);
    connect(ui->clearTypesPushButton, &QPushButton::clicked, this, &DiscTypeDistributionDialog::onClearTypes);

    validateColorMapping();

    resetTableViewToSettings();
}

void DiscTypeDistributionDialog::closeEvent(QCloseEvent* event)
{
    onCancel();
    event->ignore();
}

void DiscTypeDistributionDialog::onOK()
{
    const auto& discTypeDistribution = convertInputsToDiscTypeDistribution();

    try
    {
        GlobalSettings::get().setDiscTypeDistribution(discTypeDistribution);
        emit discDistributionChanged();
        hide();
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, "Error", e.what());
    }
}

void DiscTypeDistributionDialog::onCancel()
{
    resetTableViewToSettings();
    hide();
}

void DiscTypeDistributionDialog::onAddType()
{
    std::string name = "Disc" + std::to_string(discTypesModel_->rowCount() + 1);
    addTableViewRowFromDiscType(DiscType(name, sf::Color::White, 10, 10));
}

void DiscTypeDistributionDialog::onClearTypes()
{
    discTypesModel_->removeRows(0, discTypesModel_->rowCount());
}

void DiscTypeDistributionDialog::onDeleteDiscType()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (!button)
        return;

    QModelIndex index = ui->discDistributionTableView->indexAt(button->pos());

    discTypesModel_->removeRow(index.row());
}

void DiscTypeDistributionDialog::validateColorMapping()
{
    for (const auto& color : SupportedDiscColors)
    {
        if (!ColorNameMapping.contains(color))
            throw std::runtime_error(
                "SupportedDiscColors contains a color that is not contained in ColorNameMapping, pls fix");
    }
}

void DiscTypeDistributionDialog::addTableViewRowFromDiscType(const DiscType& discType, int percentage)
{
    using Utility::addSpinBoxToLastRow;

    int oldRowCount = discTypesModel_->rowCount();
    int newRowCount = discTypesModel_->rowCount() + 1;

    QList<QStandardItem*> items;
    for (int i = 0; i < discTypesModel_->columnCount(); ++i)
        items.append(new QStandardItem());
    discTypesModel_->appendRow(items);

    // Disc type name
    QLineEdit* nameLineEdit = new QLineEdit();
    nameLineEdit->setText(QString::fromStdString(discType.name_));
    ui->discDistributionTableView->setIndexWidget(discTypesModel_->index(oldRowCount, 0), nameLineEdit);

    // Radius
    addSpinBoxToLastRow(discType.radius_, DiscTypeLimits::MinRadius, DiscTypeLimits::MaxRadius,
                        ui->discDistributionTableView, discTypesModel_, 1,
                        QString("radius") + QString::number(newRowCount));

    // Mass
    addSpinBoxToLastRow(discType.mass_, DiscTypeLimits::MinMass, DiscTypeLimits::MaxMass, ui->discDistributionTableView,
                        discTypesModel_, 2, QString("mass") + QString::number(newRowCount));

    // Color
    QComboBox* colorComboBox = new QComboBox();
    colorComboBox->addItems(SupportedDiscColorNames);
    colorComboBox->setCurrentIndex(SupportedDiscColors.indexOf(discType.color_));
    ui->discDistributionTableView->setIndexWidget(discTypesModel_->index(oldRowCount, 3), colorComboBox);

    // Percentage
    addSpinBoxToLastRow(percentage, 0, 100, ui->discDistributionTableView, discTypesModel_, 4,
                        QString("percentage") + QString::number(newRowCount));

    // Delete
    QPushButton* deleteButton = new QPushButton("Delete");
    connect(deleteButton, &QPushButton::clicked, this, &DiscTypeDistributionDialog::onDeleteDiscType);
    ui->discDistributionTableView->setIndexWidget(discTypesModel_->index(oldRowCount, 5), deleteButton);
}

std::map<DiscType, int> DiscTypeDistributionDialog::convertInputsToDiscTypeDistribution() const
{
    std::map<DiscType, int> discTypeDistribution;

    for (int row = 0; row < discTypesModel_->rowCount(); ++row)
    {
        DiscType newType;
        int percentage = 0;

        for (int col = 0; col < discTypesModel_->columnCount(); ++col)
        {
            QWidget* widget = ui->discDistributionTableView->indexWidget(discTypesModel_->index(row, col));

            if (QLineEdit* lineEdit = qobject_cast<QLineEdit*>(widget))
            {
                // Only name has a line edit
                newType.name_ = lineEdit->text().toStdString();
            }
            else if (QSpinBox* spinBox = qobject_cast<QSpinBox*>(widget))
            {
                // Spin boxes are used for radius, mass and percentage
                if (spinBox->objectName().startsWith("radius"))
                    newType.radius_ = spinBox->value();
                else if (spinBox->objectName().startsWith("mass"))
                    newType.mass_ = spinBox->value();
                else if (spinBox->objectName().startsWith("percentage"))
                    percentage = spinBox->value();
            }
            else if (QComboBox* comboBox = qobject_cast<QComboBox*>(widget))
            {
                QString colorName = comboBox->currentText();
                newType.color_ = NameColorMapping[colorName];
            }
        }

        discTypeDistribution[newType] = percentage;
    }

    return discTypeDistribution;
}

void DiscTypeDistributionDialog::resetTableViewToSettings()
{
    onClearTypes();
    for (const auto& [discType, percentage] : GlobalSettings::getSettings().discTypeDistribution_)
        addTableViewRowFromDiscType(discType, percentage);
}
