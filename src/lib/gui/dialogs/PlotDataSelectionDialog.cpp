#include "dialogs/PlotDataSelectionDialog.hpp"
#include "core/AbstractSimulationBuilder.hpp"
#include "core/PlotCategories.hpp"
#include "core/Utility.hpp"
#include "models/PlotModel.hpp"
#include "ui_PlotDataSelectionDialog.h"
#include "widgets/MultiSelectListWidget.hpp"

#include <QCloseEvent>
#include <QMessageBox>

PlotDataSelectionDialog::PlotDataSelectionDialog(QWidget* parent, AbstractSimulationBuilder* abstractSimulationBuilder,
                                                 PlotModel* plotModel)
    : QDialog(parent)
    , ui(new Ui::PlotDataSelectionDialog)
    , abstractSimulationBuilder_(abstractSimulationBuilder)
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

void PlotDataSelectionDialog::showEvent(QShowEvent* event)
{
    const auto& discTypes = abstractSimulationBuilder_->getSimulationConfig().discTypes;
    ui->selectedDiscTypesListWidget->clear();

    for (const auto& discType : discTypes)
    {
        auto* item = new QListWidgetItem(QString::fromStdString(discType.name));
        ui->selectedDiscTypesListWidget->addItem(item);

        if (plotModel_->getActivePlotDiscTypesMap().at(discType.name))
            item->setSelected(true);
    }
}