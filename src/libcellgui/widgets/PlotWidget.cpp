#include "PlotWidget.hpp"
#include "ExceptionWithLocation.hpp"
#include "Utility.hpp"

#include <algorithm>

PlotWidget::PlotWidget(QWidget* parent)
    : QCustomPlot(parent)
{
    setInteraction(QCP::iRangeDrag, true); // Allow dragging the plot by left click-hold
    setInteraction(QCP::iRangeZoom, true); // Allow zoom with mouse wheel

    plotLayout()->insertRow(0);
    plotLayout()->addElement(0, 0, plotTitle_);

    xAxis->setLabel("t [s]");

    addLayer("legend layer");
    legend->setLayer("legend layer");
    legend->setVisible(true);

    // Place the legend
    axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop | Qt::AlignCenter);

    // These are the default colors for matplotlib. I like them.
    colors_ << QColor(31, 119, 180) << QColor(255, 127, 14) << QColor(44, 160, 44) << QColor(214, 39, 40)
            << QColor(148, 103, 189) << QColor(140, 86, 75) << QColor(227, 119, 194) << QColor(127, 127, 127)
            << QColor(188, 189, 34) << QColor(23, 190, 207);

    reset();
}

void PlotWidget::reset()
{
    xMin_ = 0;
    xMax_ = INT_MIN;
    yMin_ = INT_MAX;
    yMax_ = INT_MIN;

    // These seem to be the default values used by QCustomPlot
    yAxis->setRange(0, 5);
    xAxis->setRange(0, 5);

    clearGraphs();
    graphs_.clear();
    sumGraph_ = nullptr;

    // TODO

    setAxisLabels();
}

void PlotWidget::replacePlot(const QVector<cell::DiscTypeMap<double>>& dataPoints)
{
    reset();

    if (dataPoints.empty())
    {
        replot();
        return;
    }

    for (auto i = 0; i < dataPoints.size(); ++i)
        plotDataPoint(dataPoints[i], i == dataPoints.size() - 1);
}

void PlotWidget::plotDataPoint(const cell::DiscTypeMap<double>& dataPoint, bool doReplot)
{
    // TODO

    if (doReplot)
    {
        yAxis->setRange(yMin_ - 1, yMax_ + 1);
        xAxis->setRange(xMin_, xMax_);

        replot();
    }
}

void PlotWidget::addDataPoint(const cell::DiscTypeMap<double>& dataPoint)
{
    if (graphs_.empty())
        return;

    const auto& size = graphs_.begin()->second->dataCount();
}

void PlotWidget::createSumGraph()
{
    sumGraph_ = addGraph();
    sumGraph_->setPen(QColor());
    legend->setVisible(false);
}

void PlotWidget::createRegularGraphs()
{
    legend->setVisible(true);

    for (int i = 0; i < legend->itemCount(); ++i)
        legend->item(i)->setLayer("legend layer");
}

void PlotWidget::setAxisLabels()
{
}

void PlotWidget::addDataPointSum(const cell::DiscTypeMap<double>& dataPoint)
{
    if (sumGraph_ == nullptr)
        throw ExceptionWithLocation(
            "Can't add data point to sumGraph_: Is nullptr (this is a bug and shouldn't happen");

    const auto& size = sumGraph_->dataCount();
}

void PlotWidget::setModel(PlotModel* plotModel)
{
    connect(plotModel, &PlotModel::dataPointAdded,
            [this](const cell::DiscTypeMap<double>& dataPoint) { plotDataPoint(dataPoint); });
    connect(plotModel, &PlotModel::newPlotCreated, this, &PlotWidget::replacePlot);
}
