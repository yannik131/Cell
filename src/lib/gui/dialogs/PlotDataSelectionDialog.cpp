#include "dialogs/PlotDataSelectionDialog.hpp"
#include "core/PlotCategories.hpp"
#include "ui_PlotDataSelectionDialog.h"
#include "widgets/MultiSelectListWidget.hpp"

#include <QCloseEvent>
#include <QMessageBox>

PlotDataSelectionDialog::PlotDataSelectionDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::PlotDataSelectionDialog)
{
    ui->setupUi(this);

    connect(ui->deselectAllButton, &QPushButton::clicked, ui->selectedDiscTypesListWidget,
            &QListWidget::clearSelection);
    connect(ui->selectAllButton, &QPushButton::clicked, ui->selectedDiscTypesListWidget, &QListWidget::selectAll);

    connect(ui->doneButton, &QPushButton::clicked, this, &PlotDataSelectionDialog::saveSettings);
}

void PlotDataSelectionDialog::closeEvent(QCloseEvent*)
{
    hide();
    loadSettings();
}

void PlotDataSelectionDialog::saveSettings()
{
    const auto& selectedDiscTypeNames = ui->selectedDiscTypesListWidget->getSelectedNames();

    try
    {
        hide();
    }
    catch (const std::runtime_error& e)
    {
        QMessageBox::critical(this, "Error", e.what());
    }
}

void PlotDataSelectionDialog::loadSettings()
{
    ui->selectedDiscTypesListWidget->clear();
}
