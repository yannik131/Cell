#include "AnalysisPlotWidget.hpp"
#include "GlobalSettings.hpp"

#include <algorithm>

AnalysisPlotWidget::AnalysisPlotWidget(QWidget* parent)
    : QCustomPlot(parent)
{
    setInteraction(QCP::iRangeDrag, true); // Allow dragging the plot by left click-hold
    setInteraction(QCP::iRangeZoom, true); // Allow zoom with mouse wheel

    plotTitle_ = new QCPTextElement(this, "Collisions per second", QFont("sans", 12, QFont::Bold));
    plotLayout()->insertRow(0);
    plotLayout()->addElement(0, 0, plotTitle_);

    xAxis->setLabel("t [s]");
    yAxis->setLabel("N");

    connect(plotDataModel_, &PlotDataModel::plotData, this, &AnalysisPlotWidget::plot);
}

void AnalysisPlotWidget::reset()
{
    yMax_ = 0;
    xData_.clear();
    yData_.clear();
    clearGraphs();
    replot();
}

void AnalysisPlotWidget::plot(const PlotData& plotData)
{
    clearGraphs();
    if (plotData.collisionCounts_.empty())
        return;

    QVector<double> x;
    for (int i = 0; i < plotData.collisionCounts_.size(); ++i)
        x.push_back(i * GlobalSettings::getSettings().plotTimeInterval_.asSeconds());

    int yMax = *std::max_element(plotData.collisionCounts_.begin(), plotData.collisionCounts_.end());

    QCPGraph* graph = addGraph();
    graph->setPen(QPen(QColor(31, 119, 180)));
    graph->setData(x, plotData.collisionCounts_, true);

    yAxis->setRange(0, yMax);
    xAxis->setRange(0, x.back());

    replot();
}

PlotDataModel* AnalysisPlotWidget::plotDataModel() const
{
    return plotDataModel_;
}
