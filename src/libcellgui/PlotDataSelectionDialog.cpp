#include "PlotDataSelectionDialog.hpp"
#include "MultiSelectListWidget.hpp"
#include "PlotCategories.hpp"
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
    connect(ui->doneButton, &QPushButton::clicked, this, &PlotDataSelectionDialog::emitSelectedDiscTypes);

    setAndSelectDiscTypes(SupportedPlotCategoryNames);
}

void PlotDataSelectionDialog::emitSelectedDiscTypes()
{
    const auto& selectedDiscTypeNames = ui->selectedDiscTypesListWidget->getSelectedNames();

    emit PlotDataSelectionDialog::selectedDiscTypeNames(selectedDiscTypeNames);
}

void PlotDataSelectionDialog::setAndSelectDiscTypes(const QStringList& discTypes)
{
    ui->selectedDiscTypesListWidget->clear();

    for (const auto& discType : discTypes)
        ui->selectedDiscTypesListWidget->addItem(discType);

    ui->selectedDiscTypesListWidget->selectAll();
}