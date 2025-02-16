#include "DiscDistributionDialog.hpp"
#include "ColorMapping.hpp"
#include "DiscType.hpp"
#include "GlobalSettings.hpp"
#include "ui_DiscDistributionDialog.h"

#include <QCloseEvent>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>

#include <map>

#include <glog/logging.h>

DiscDistributionDialog::DiscDistributionDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::DiscDistributionDialog)
{
    ui->setupUi(this);
    model_ = new QStandardItemModel(this);
    model_->setColumnCount(6);
    model_->setHeaderData(0, Qt::Horizontal, "Disc type");
    model_->setHeaderData(1, Qt::Horizontal, "Radius [px]");
    model_->setHeaderData(2, Qt::Horizontal, "Mass");
    model_->setHeaderData(3, Qt::Horizontal, "Color");
    model_->setHeaderData(4, Qt::Horizontal, "Percentage");
    model_->setHeaderData(5, Qt::Horizontal, "Delete");
    ui->discDistributionTableView->setModel(model_);

    connect(ui->okPushButton, &QPushButton::clicked, this, &DiscDistributionDialog::onOK);
    connect(ui->cancelPushButton, &QPushButton::clicked, this, &DiscDistributionDialog::onCancel);
    connect(ui->addPushButton, &QPushButton::clicked, this, &DiscDistributionDialog::onAdd);
    connect(ui->clearPushButton, &QPushButton::clicked, this, &DiscDistributionDialog::onClear);

    validateColorMapping();

    for (const auto& [discType, percentage] : GlobalSettings::getSettings().discTypeDistribution_)
        addTableViewRowFromDiscType(discType, percentage);
}

void DiscDistributionDialog::closeEvent(QCloseEvent* event)
{
    onCancel();
    event->ignore();
}

void DiscDistributionDialog::onOK()
{
    std::map<DiscType, int> discTypeDistribution;

    // Convert the entries into DiscType objects
    for (int row = 0; row < model_->rowCount(); ++row)
    {
        DiscType newType;
        int percentage = 0;

        for (int col = 0; col < model_->columnCount(); ++col)
        {
            QWidget* widget = ui->discDistributionTableView->indexWidget(model_->index(row, col));

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

void DiscDistributionDialog::onCancel()
{
    onClear();
    for (const auto& [discType, percentage] : GlobalSettings::getSettings().discTypeDistribution_)
        addTableViewRowFromDiscType(discType, percentage);
    hide();
}

void DiscDistributionDialog::onAdd()
{
    std::string name = "Disc" + std::to_string(model_->rowCount() + 1);
    addTableViewRowFromDiscType(DiscType(name, sf::Color::White, 10, 10));
}

void DiscDistributionDialog::onClear()
{
    model_->removeRows(0, model_->rowCount());
}

void DiscDistributionDialog::onDeleteDiscType()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (!button)
        return;

    QModelIndex index = ui->discDistributionTableView->indexAt(button->pos());

    model_->removeRow(index.row());
}

void DiscDistributionDialog::validateColorMapping()
{
    for (const auto& color : SupportedDiscColors)
    {
        if (!ColorNameMapping.contains(color))
            throw std::runtime_error(
                "SupportedDiscColors contains a color that is not contained in ColorNameMapping, pls fix");
    }
}

void DiscDistributionDialog::addTableViewRowFromDiscType(const DiscType& discType, int percentage)
{
    int oldRowCount = model_->rowCount();
    int newRowCount = model_->rowCount() + 1;

    QList<QStandardItem*> items;
    for (int i = 0; i < 6; ++i)
        items.append(new QStandardItem());
    model_->appendRow(items);

    // Disc type name
    QLineEdit* nameLineEdit = new QLineEdit();
    nameLineEdit->setText(QString::fromStdString(discType.name_));
    ui->discDistributionTableView->setIndexWidget(model_->index(oldRowCount, 0), nameLineEdit);

    // Radius
    QSpinBox* radiusSpinBox = new QSpinBox();
    radiusSpinBox->setObjectName(QString("radius") + QString::number(newRowCount));
    radiusSpinBox->setValue(discType.radius_);
    radiusSpinBox->setRange(DiscTypeLimits::MinRadius, DiscTypeLimits::MaxRadius);
    ui->discDistributionTableView->setIndexWidget(model_->index(oldRowCount, 1), radiusSpinBox);

    // Mass
    QSpinBox* massSpinBox = new QSpinBox();
    massSpinBox->setObjectName(QString("mass") + QString::number(newRowCount));
    massSpinBox->setValue(discType.mass_);
    massSpinBox->setRange(DiscTypeLimits::MinMass, DiscTypeLimits::MaxMass);
    ui->discDistributionTableView->setIndexWidget(model_->index(oldRowCount, 2), massSpinBox);

    // Color
    QComboBox* colorComboBox = new QComboBox();
    colorComboBox->addItems(SupportedDiscColorNames);
    colorComboBox->setCurrentIndex(SupportedDiscColors.indexOf(discType.color_));
    ui->discDistributionTableView->setIndexWidget(model_->index(oldRowCount, 3), colorComboBox);

    // Percentage
    QSpinBox* percentageSpinBox = new QSpinBox();
    percentageSpinBox->setObjectName(QString("percentage") + QString::number(newRowCount));
    percentageSpinBox->setValue(percentage);
    percentageSpinBox->setRange(0, 100);
    ui->discDistributionTableView->setIndexWidget(model_->index(oldRowCount, 4), percentageSpinBox);

    // Delete
    QPushButton* deleteButton = new QPushButton("Delete");
    connect(deleteButton, &QPushButton::clicked, this, &DiscDistributionDialog::onDeleteDiscType);
    ui->discDistributionTableView->setIndexWidget(model_->index(oldRowCount, 5), deleteButton);
}
