#include "widgets/PlotControlWidget.hpp"
#include "PlotControlWidget.hpp"
#include "core/PlotCategories.hpp"
#include "ui_PlotControlWidget.h"

PlotControlWidget::PlotControlWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::PlotControlWidget)
{
    ui->setupUi(this);

    // Remove items set by the designer, might not be up to date (source code is the ssot)
    ui->plotTypeComboBox->clear();
    ui->plotTypeComboBox->addItems(SupportedPlotCategoryNames());
    connect(ui->selectDiscTypesPushButton, &QPushButton::clicked, this, [&]() { emit selectDiscTypesClicked(); });
    plotTimeIntervalValueTimer_.setInterval(2000);
    connect(&plotTimeIntervalValueTimer_, &QTimer::timeout, this,
            &PlotControlWidget::displayActualPlotTimeIntervalValue);
}

PlotControlWidget::~PlotControlWidget() = default;

void PlotControlWidget::setModel(PlotModel* plotModel)
{
    connect(ui->plotSumCheckBox, &QCheckBox::toggled, plotModel, &PlotModel::setPlotSum);
    connect(ui->interpolateCheckBox, &QCheckBox::toggled, plotModel, &PlotModel::setInterpolate);
    connect(ui->plotTypeComboBox, &QComboBox::currentTextChanged,
            [plotModel](const QString& text) { plotModel->setPlotCategory(PlotCategoryNameMap()[text]); });
    connect(ui->plotTimeIntervalSpinBox, &QSpinBox::valueChanged,
            [plotModel, this](int value)
            {
                plotModel->setPlotTimeInterval(roundedToNearestMultipleOfTen(value));
                plotTimeIntervalValueTimer_.stop();
                plotTimeIntervalValueTimer_.start();
            });
    ui->plotTimeIntervalSpinBox->setValue(
        ch::duration_cast<ch::milliseconds>(plotModel->getPlotTimeInterval()).count());
}

void PlotControlWidget::displayActualPlotTimeIntervalValue()
{
    int value = ui->plotTimeIntervalSpinBox->value();
    ui->plotTimeIntervalSpinBox->setValue(roundedToNearestMultipleOfTen(value));
}

int PlotControlWidget::roundedToNearestMultipleOfTen(int value)
{
    if (value < 10)
        return 10;

    if (value % 10 != 0)
        return static_cast<int>(qRound(value / 10.0) * 10);

    return value;
}
