#include "PlotDataSelectionDialog.hpp"
#include "GlobalGUISettings.hpp"
#include "GlobalSettings.hpp"
#include "MultiSelectListWidget.hpp"
#include "PlotCategories.hpp"
#include "ui_PlotDataSelectionDialog.h"

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

    connect(&GlobalGUISettings::get(), &GlobalGUISettings::plotDataMapUpdated, this,
            &PlotDataSelectionDialog::loadSettings);
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
        GlobalGUISettings::get().setDiscTypesPlotMap(selectedDiscTypeNames);
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

    for (const auto& [discType, _] : cell::GlobalSettings::getSettings().discTypeDistribution_)
    {
        auto item = new QListWidgetItem(QString::fromStdString(discType.getName()));
        ui->selectedDiscTypesListWidget->addItem(item);

        item->setSelected(GlobalGUISettings::getGUISettings().discTypesPlotMap_.at(discType));
    }
}
