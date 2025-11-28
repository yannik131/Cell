#include "dialogs/PlotDataSelectionDialog.hpp"
#include "core/PlotCategories.hpp"
#include "core/SimulationConfigUpdater.hpp"
#include "core/Utility.hpp"
#include "models/PlotModel.hpp"
#include "ui_PlotDataSelectionDialog.h"
#include "widgets/MultiSelectListWidget.hpp"

#include <QCloseEvent>
#include <QMessageBox>

PlotDataSelectionDialog::PlotDataSelectionDialog(QWidget* parent, SimulationConfigUpdater* simulationConfigUpdater,
                                                 PlotModel* plotModel)
    : QDialog(parent)
    , ui(new Ui::PlotDataSelectionDialog)
    , simulationConfigUpdater_(simulationConfigUpdater)
    , plotModel_(plotModel)
{
    ui->setupUi(this);

    connect(ui->deselectAllButton, &QPushButton::clicked, ui->selectedDiscTypesListWidget,
            &QListWidget::clearSelection);
    connect(ui->selectAllButton, &QPushButton::clicked, ui->selectedDiscTypesListWidget, &QListWidget::selectAll);

    connect(ui->doneButton, &QPushButton::clicked,
            utility::safeSlot(this,
                              [&]()
                              {
                                  std::vector<std::string> activeDiscTypeNames;
                                  for (const auto& discTypeName : ui->selectedDiscTypesListWidget->getSelectedNames())
                                      activeDiscTypeNames.push_back(discTypeName.toStdString());

                                  plotModel_->setActivePlotDiscTypes(activeDiscTypeNames);
                                  accept();
                              }));
}

PlotDataSelectionDialog::~PlotDataSelectionDialog() = default;

void PlotDataSelectionDialog::showEvent(QShowEvent*)
{
    const auto& discTypes = simulationConfigUpdater_->getSimulationConfig().discTypes;
    ui->selectedDiscTypesListWidget->clear();

    for (const auto& discType : discTypes)
    {
        auto* item = new QListWidgetItem(QString::fromStdString(discType.name));
        ui->selectedDiscTypesListWidget->addItem(item);

        if (plotModel_->getActivePlotDiscTypesMap().at(discType.name))
            item->setSelected(true);
    }
}