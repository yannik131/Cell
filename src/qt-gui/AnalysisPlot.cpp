#include "AnalysisPlot.hpp"

AnalysisPlot::AnalysisPlot(QWidget* parent) : QCustomPlot(parent)
{
    setInteraction(QCP::iRangeDrag, true); // Allow dragging the plot by left click-hold
    setInteraction(QCP::iRangeZoom, true); // Allow zoom with mouse wheel

    plotTitle_ = new QCPTextElement(this, "Collisions per second", QFont("sans", 12, QFont::Bold));
    plotLayout()->insertRow(0);
    plotLayout()->addElement(0, 0, plotTitle_);
    
    xAxis->setLabel("t [s]");
    yAxis->setLabel("N");
}

void AnalysisPlot::addDataPoint(double y)
{
    if(y > yMax_)
        yMax_ = y;

    xData_.append(xData_.size());
    yData_.append(y);
    clearGraphs();
    QCPGraph* graph = addGraph();
    graph->setPen(QPen(QColor(31, 119, 180)));
    graph->setData(xData_, yData_, true);

    yAxis->setRange(0, yMax_);
    xAxis->setRange(0, xData_.size() + 1);

    replot();
}

void AnalysisPlot::reset()
{
    xData_.clear();
    yData_.clear();
    clearGraphs();
    replot();
}
