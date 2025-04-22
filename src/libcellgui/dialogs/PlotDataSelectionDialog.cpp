#include "PlotDataSelectionDialog.hpp"
#include "GlobalGUISettings.hpp"
#include "GlobalSettings.hpp"
#include "GlobalSettingsFunctor.hpp"
#include "MultiSelectListWidget.hpp"
#include "PlotCategories.hpp"
#include "ui_PlotDataSelectionDialog.h"

PlotDataSelectionDialog::PlotDataSelectionDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::PlotDataSelectionDialog)
{
    ui->setupUi(this);

    connect(ui->deselectAllButton, &QPushButton::clicked, ui->selectedDiscTypesListWidget,
            &QListWidget::clearSelection);
    connect(ui->selectAllButton, &QPushButton::clicked, ui->selectedDiscTypesListWidget, &QListWidget::selectAll);

    connect(ui->doneButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(ui->doneButton, &QPushButton::clicked, this, &PlotDataSelectionDialog::saveSettings);

    loadSettings();
    connect(&GlobalSettingsFunctor::get(), &GlobalSettingsFunctor::discTypeDistributionChanged, this,
            &PlotDataSelectionDialog::loadSettings);
}

void PlotDataSelectionDialog::saveSettings()
{
    const auto& selectedDiscTypeNames = ui->selectedDiscTypesListWidget->getSelectedNames();

    GlobalGUISettings::get().setDiscTypesPlotMap(selectedDiscTypeNames);
}

void PlotDataSelectionDialog::loadSettings()
{
    ui->selectedDiscTypesListWidget->clear();

    for (const auto& [discType, _] : GlobalSettings::getSettings().discTypeDistribution_)
        ui->selectedDiscTypesListWidget->addItem(QString::fromStdString(discType.getName()));

    ui->selectedDiscTypesListWidget->selectAll();
}