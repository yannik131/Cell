#include "widgets/PlotControlWidget.hpp"
#include "core/PlotCategories.hpp"
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

void PlotControlWidget::setModel(PlotModel* plotModel)
{
    connect(ui->plotSumCheckBox, &QCheckBox::toggled, plotModel, &PlotModel::setPlotSum);
    connect(ui->plotTypeComboBox, &QComboBox::currentTextChanged,
            [plotModel](const QString& text) { plotModel->setPlotCategory(PlotCategoryNameMapping[text]); });
    connect(ui->plotTimeIntervalSpinBox, &QSpinBox::valueChanged, plotModel, &PlotModel::setPlotTimeInterval);
}
