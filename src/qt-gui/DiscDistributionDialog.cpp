#include "DiscDistributionDialog.hpp"
#include "DiscType.hpp"
#include "GlobalSettings.hpp"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>

#include <map>

DiscDistributionDialog::DiscDistributionDialog(QWidget* parent)
    : QDialog(parent)
{
    model_ = new QStandardItemModel(this);
    model_->setColumnCount(6);
    model_->setHeaderData(0, Qt::Horizontal, "Disc type");
    model_->setHeaderData(1, Qt::Horizontal, "Radius [px]");
    model_->setHeaderData(2, Qt::Horizontal, "Mass");
    model_->setHeaderData(3, Qt::Horizontal, "Color");
    model_->setHeaderData(4, Qt::Horizontal, "Percentage");
    model_->setHeaderData(5, Qt::Horizontal, "Delete");

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &DiscDistributionDialog::onOK);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &DiscDistributionDialog::onCancel);
    connect(ui->addPushButton, &QPushButton::clicked, this, &DiscDistributionDialog::onAdd);
    connect(ui->clearPushButton, &QPushButton::clicked, this, &DiscDistributionDialog::onClear);

    validateColorMapping();
}

void DiscDistributionDialog::onOK()
{
    if (model_->rowCount() == 0)
    {
        QMessageBox::critical(this, "Error", "Distribution cannot be empty");
        return;
    }

    std::map<DiscType, int> discTypeDistribution;

    for (int row = 0; row < model_->rowCount(); ++row)
    {
        DiscType newType;
        int percentage;

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
            else if (QPushButton* button = qobject_cast<QPushButton*>(widget))
            {
                // Handle button if necessary, but usually buttons don't hold data
            }
        }
    }
}

void DiscDistributionDialog::onCancel()
{
}

void DiscDistributionDialog::onAdd()
{
    int oldRowCount = model_->rowCount();
    int newRowCount = model_->rowCount() + 1;

    QList<QStandardItem*> items;
    for (int i = 0; i < 6; ++i)
        items.append(new QStandardItem());

    // Disc type name
    QString defaultName = QString("Disc") + QString::number(newRowCount);
    QLineEdit* nameLineEdit = new QLineEdit();
    nameLineEdit->setText(defaultName);
    ui->discDistributionTableView->setIndexWidget(model_->index(oldRowCount, 0), nameLineEdit);

    // Radius
    QSpinBox* radiusSpinBox = new QSpinBox();
    radiusSpinBox->setObjectName(QString("Radius") + QString::number(newRowCount));
    radiusSpinBox->setValue(10);
    ui->discDistributionTableView->setIndexWidget(model_->index(oldRowCount, 1), radiusSpinBox);

    // Mass
    QSpinBox* massSpinBox = new QSpinBox();
    massSpinBox->setObjectName(QString("mass") + QString::number(newRowCount));
    massSpinBox->setValue(10);
    ui->discDistributionTableView->setIndexWidget(model_->index(oldRowCount, 2), massSpinBox);

    // Color
    QComboBox* colorComboBox = new QComboBox();
    colorComboBox->addItems(ColorNameMapping.values());
    ui->discDistributionTableView->setIndexWidget(model_->index(oldRowCount - 1, 3), colorComboBox);

    // Percentage
    QSpinBox* percentageSpinBox = new QSpinBox();
    percentageSpinBox->setObjectName(QString("percentage") + QString::number(newRowCount));
    percentageSpinBox->setValue(10);
    ui->discDistributionTableView->setIndexWidget(model_->index(oldRowCount, 4), percentageSpinBox);

    // Delete
    QPushButton* deleteButton = new QPushButton("Delete");
    connect(deleteButton, &QPushButton::clicked, this, &DiscDistributionDialog::onDeleteDiscType);
    ui->discDistributionTableView->setIndexWidget(model_->index(oldRowCount, 5), deleteButton);

    model_->appendRow(items);
}

void DiscDistributionDialog::onClear()
{
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
    for (const auto& color : SupportedDicsColors)
    {
        if (!ColorNameMapping.contains(color))
            throw std::runtime_error(
                "SupportedDiscColors contains a color that is not contained in ColorNameMapping, pls fix");
    }
}
