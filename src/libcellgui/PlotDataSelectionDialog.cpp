#include "PlotDataSelectionDialog.hpp"
#include "MultiSelectListWidget.hpp"
#include "ui_PlotDataSelectionDialog.h"

#include <algorithm>
#include <regex>

PlotDataSelectionDialog::PlotDataSelectionDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::PlotDataSelectionDialog)
{
    ui->setupUi(this);

    connect(ui->deselectAllButton, &QPushButton::clicked, ui->selectedDiscTypesListWidget,
            &QListWidget::clearSelection);
    connect(ui->selectAllButton, &QPushButton::clicked, ui->selectedDiscTypesListWidget, &QListWidget::selectAll);

    connect(ui->doneButton, &QPushButton::clicked, this, &QDialog::accept);
}

void PlotDataSelectionDialog::setAndSelectDiscTypes(const QStringList& discTypes)
{
    ui->selectedDiscTypesListWidget->clear();

    for (const auto& discType : discTypes)
        ui->selectedDiscTypesListWidget->addItem(discType);

    ui->selectedDiscTypesListWidget->selectAll();
}

QStringList PlotDataSelectionDialog::getSelectedDiscTypeNames() const
{
    return ui->selectedDiscTypesListWidget->getSelectedNames();
}
