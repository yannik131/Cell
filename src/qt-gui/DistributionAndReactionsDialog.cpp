#include "DistributionAndReactionsDialog.hpp"
#include "ColorMapping.hpp"
#include "DiscType.hpp"
#include "GlobalSettings.hpp"
#include "ui_DistributionAndReactionsDialog.h"

#include <QCloseEvent>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>

#include <map>

#include <glog/logging.h>

DistributionAndReactionsDialog::DistributionAndReactionsDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::DistributionAndReactionsDialog)
    , discDistributionModel_(new QStandardItemModel(this))
    , reactionsModel_(new QStandardItemModel(this))
{
    ui->setupUi(this);

    setModelHeaderData(discDistributionModel_, {"Disc type", "Radius [px]", "Mass", "Color", "Percentage", "Delete"});
    setModelHeaderData(reactionsModel_, {"A", "+", "B", "->", "C", "+", "D"});

    ui->discDistributionTableView->setModel(discDistributionModel_);
    ui->reactionsTableView->setModel(reactionsModel_);

    connect(ui->okPushButton, &QPushButton::clicked, this, &DistributionAndReactionsDialog::onOK);
    connect(ui->cancelPushButton, &QPushButton::clicked, this, &DistributionAndReactionsDialog::onCancel);
    connect(ui->addPushButton, &QPushButton::clicked, this, &DistributionAndReactionsDialog::onAdd);
    connect(ui->clearPushButton, &QPushButton::clicked, this, &DistributionAndReactionsDialog::onClear);

    validateColorMapping();

    for (const auto& [discType, percentage] : GlobalSettings::getSettings().discTypeDistribution_)
        addTableViewRowFromDiscType(discType, percentage);
}

void DistributionAndReactionsDialog::closeEvent(QCloseEvent* event)
{
    onCancel();
    event->ignore();
}

void DistributionAndReactionsDialog::onOK()
{
    std::map<DiscType, int> discTypeDistribution;

    // Convert the entries into DiscType objects
    for (int row = 0; row < discDistributionModel_->rowCount(); ++row)
    {
        DiscType newType;
        int percentage = 0;

        for (int col = 0; col < discDistributionModel_->columnCount(); ++col)
        {
            QWidget* widget = ui->discDistributionTableView->indexWidget(discDistributionModel_->index(row, col));

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

void DistributionAndReactionsDialog::onCancel()
{
    onClear();
    for (const auto& [discType, percentage] : GlobalSettings::getSettings().discTypeDistribution_)
        addTableViewRowFromDiscType(discType, percentage);
    hide();
}

void DistributionAndReactionsDialog::onAdd()
{
    std::string name = "Disc" + std::to_string(discDistributionModel_->rowCount() + 1);
    addTableViewRowFromDiscType(DiscType(name, sf::Color::White, 10, 10));
}

void DistributionAndReactionsDialog::onClear()
{
    discDistributionModel_->removeRows(0, discDistributionModel_->rowCount());
}

void DistributionAndReactionsDialog::onDeleteDiscType()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (!button)
        return;

    QModelIndex index = ui->discDistributionTableView->indexAt(button->pos());

    discDistributionModel_->removeRow(index.row());
}

void DistributionAndReactionsDialog::validateColorMapping()
{
    for (const auto& color : SupportedDiscColors)
    {
        if (!ColorNameMapping.contains(color))
            throw std::runtime_error(
                "SupportedDiscColors contains a color that is not contained in ColorNameMapping, pls fix");
    }
}

void DistributionAndReactionsDialog::addTableViewRowFromDiscType(const DiscType& discType, int percentage)
{
    int oldRowCount = discDistributionModel_->rowCount();
    int newRowCount = discDistributionModel_->rowCount() + 1;

    QList<QStandardItem*> items;
    for (int i = 0; i < 6; ++i)
        items.append(new QStandardItem());
    discDistributionModel_->appendRow(items);

    // Disc type name
    QLineEdit* nameLineEdit = new QLineEdit();
    nameLineEdit->setText(QString::fromStdString(discType.name_));
    ui->discDistributionTableView->setIndexWidget(discDistributionModel_->index(oldRowCount, 0), nameLineEdit);

    // Radius
    QSpinBox* radiusSpinBox = new QSpinBox();
    radiusSpinBox->setObjectName(QString("radius") + QString::number(newRowCount));
    radiusSpinBox->setValue(discType.radius_);
    radiusSpinBox->setRange(DiscTypeLimits::MinRadius, DiscTypeLimits::MaxRadius);
    ui->discDistributionTableView->setIndexWidget(discDistributionModel_->index(oldRowCount, 1), radiusSpinBox);

    // Mass
    QSpinBox* massSpinBox = new QSpinBox();
    massSpinBox->setObjectName(QString("mass") + QString::number(newRowCount));
    massSpinBox->setValue(discType.mass_);
    massSpinBox->setRange(DiscTypeLimits::MinMass, DiscTypeLimits::MaxMass);
    ui->discDistributionTableView->setIndexWidget(discDistributionModel_->index(oldRowCount, 2), massSpinBox);

    // Color
    QComboBox* colorComboBox = new QComboBox();
    colorComboBox->addItems(SupportedDiscColorNames);
    colorComboBox->setCurrentIndex(SupportedDiscColors.indexOf(discType.color_));
    ui->discDistributionTableView->setIndexWidget(discDistributionModel_->index(oldRowCount, 3), colorComboBox);

    // Percentage
    QSpinBox* percentageSpinBox = new QSpinBox();
    percentageSpinBox->setObjectName(QString("percentage") + QString::number(newRowCount));
    percentageSpinBox->setValue(percentage);
    percentageSpinBox->setRange(0, 100);
    ui->discDistributionTableView->setIndexWidget(discDistributionModel_->index(oldRowCount, 4), percentageSpinBox);

    // Delete
    QPushButton* deleteButton = new QPushButton("Delete");
    connect(deleteButton, &QPushButton::clicked, this, &DistributionAndReactionsDialog::onDeleteDiscType);
    ui->discDistributionTableView->setIndexWidget(discDistributionModel_->index(oldRowCount, 5), deleteButton);
}

void DistributionAndReactionsDialog::setModelHeaderData(QStandardItemModel* model, const QStringList& headers)
{
    model->setColumnCount(headers.size());

    for (int i = 0; i < headers.size(); ++i)
        model->setHeaderData(i, Qt::Horizontal, headers[i]);
}
