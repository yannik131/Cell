#include "PlotOverviewWidget.hpp"

#include <QTimer>

PlotOverviewWidget::PlotOverviewWidget(QWidget* parent)
    : QWidget(parent)
    , plotDataSelectionDialog_(new PlotDataSelectionDialog(this))
{
    QTimer::singleShot(0, this, &PlotOverviewWidget::init);
}

void PlotOverviewWidget::init()
{
    initializeWidgets();
}

void PlotOverviewWidget::initializeWidgets()
{
    analysisPlotWidget_ = findChild<AnalysisPlotWidget*>("analysisPlotWidget");
    plotTypeComboBox_ = findChild<QComboBox*>("plotTypeComboBox");
    selectDiscTypesPushButton_ = findChild<QPushButton*>("selectDiscTypesPushButton");

    plotTypeComboBox_->clear();
    plotTypeComboBox_->addItems(SupportedPlotCategoryNames);
}

void PlotOverviewWidget::setCallbacks()
{
    connect(plotTypeComboBox_, &QComboBox::currentIndexChanged, this,
            &PlotOverviewWidget::onPlotTypeComboBoxCurrentIndexChanged);
    connect(selectDiscTypesPushButton_, &QPushButton::clicked, plotDataSelectionDialog_,
            &PlotDataSelectionDialog::show);
}

void PlotOverviewWidget::onPlotTypeComboBoxCurrentIndexChanged(int index)
{
    const PlotCategory& plotCategory = SupportedPlotCategories[index];
    analysisPlotWidget_->plotDataModel()->setCurrentPlotCategory(plotCategory);
}
