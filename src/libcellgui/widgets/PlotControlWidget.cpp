#include "PlotControlWidget.hpp"
#include "PlotCategories.hpp"
#include "ui_PlotControlWidget.h"

PlotControlWidget::PlotControlWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::PlotControlWidget)
{
    ui->setupUi(this);

    // Remove items set by the designer, might not be up to date (source code is the ssot)
    ui->plotTypeComboBox->clear();
    ui->plotTypeComboBox->addItems(SupportedPlotCategoryNames);
}
