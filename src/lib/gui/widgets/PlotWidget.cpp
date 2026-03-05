#include "widgets/PlotWidget.hpp"
#include "cell/ExceptionWithLocation.hpp"
#include "core/Utility.hpp"

#include "PlotWidget.hpp"
#include <QFont>

PlotWidget::PlotWidget(QWidget* parent)
    : QCustomPlot(parent)
{
    xAxis->setLabel("t [s]");

    addLayer("legend layer");
    legend->setLayer("legend layer");
    legend->setVisible(true);

    // Place the legend
    axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop | Qt::AlignRight);
}

void PlotWidget::createLinePlots(const std::vector<std::string>& labels, const std::vector<sf::Color>& colors)
{
    reset();

    if (labels.size() != colors.size())
        throw ExceptionWithLocation("Must have equal number of labels and colors");

    for (std::size_t i = 0; i < labels.size(); ++i)
    {
        QCPGraph* graph = addGraph();
        if (colors[i] == sf::Color::White)
            graph->setPen(utility::sfColorToQColor(sf::Color::Black));
        else
            graph->setPen(utility::sfColorToQColor(colors[i]));

        graph->setName(QString::fromStdString(labels[i]));
        graphs_[labels[i]] = graph;
    }

    updateLegend(labels);
    enableZoom(true);

    replot();
}

void PlotWidget::createHistogram(const std::vector<std::string>& labels, const std::vector<sf::Color>& colors,
                                 const Histogram& histogram)
{
    reset();

    if (labels.size() != colors.size())
        throw ExceptionWithLocation("Must have equal number of labels and colors");

    const auto& ax = histogram.axis(1);
    const double binWidth = ax.bin(0).width();

    for (std::size_t i = 0; i < labels.size(); ++i)
    {
        QCPBars* graph = new QCPBars(xAxis, yAxis);
        graph->setAntialiased(false);
        graph->setStackingGap(0);

        if (colors[i] == sf::Color::White)
            graph->setBrush(utility::sfColorToQColor(sf::Color::Black));
        else
            graph->setBrush(utility::sfColorToQColor(colors[i]));

        graph->setPen(QPen(Qt::black, 1));

        graph->setName(QString::fromStdString(labels[i]));
        graph->setWidthType(QCPBars::wtPlotCoords);
        graph->setWidth(binWidth);

        histogram_[labels[i]] = graph;

        if (i > 0)
            graph->moveAbove(histogram_[labels[i - 1]]);
    }

    xAxis->setRange(ax.value(0), ax.value(ax.size()));
    xAxis->setLabel(ax.metadata().c_str());
    xAxis->setNumberFormat("f");
    xAxis->setNumberPrecision(0);

    yAxis->setLabel("Counts");
    yAxis->grid()->setSubGridVisible(true);

    updateLegend(labels);
    enableZoom(false);

    replot();
}

void PlotWidget::plotLinePlotPoint(const std::unordered_map<std::string, double>& dataPoint, double xStep,
                                   DoReplot doReplot)
{
    double x = xStep * count_++;

    for (auto& [label, graph] : graphs_)
    {
        double value = 0;
        const auto& iter = dataPoint.find(label);
        if (iter != dataPoint.end())
            value = iter->second;

        graph->addData(x, value);

        yMin_ = std::min(yMin_, value);
        yMax_ = std::max(yMax_, value);
    }

    yAxis->setRange(yMin_ - 1, yMax_ + 1);
    xAxis->setRange(xMin_, x);

    if (doReplot.value)
        replot();
}

void PlotWidget::plotLinePlotPoints(const std::vector<std::unordered_map<std::string, double>>& dataPoints,
                                    double xStep)
{
    for (const auto& dataPoint : dataPoints)
        plotLinePlotPoint(dataPoint, xStep, DoReplot{false});

    replot();
}

void PlotWidget::plotHistogram(const Histogram& histogram)
{
    const auto& categoryAxis = histogram.axis<0>();
    const auto& regularAxis = histogram.axis<1>();

    QVector<double> binCenters;
    binCenters.reserve(static_cast<qsizetype>(regularAxis.size()));
    for (int i = 0; i < regularAxis.size(); ++i)
        binCenters << regularAxis.bin(i).center();

    QVector<double> counts;
    counts.reserve(static_cast<qsizetype>(regularAxis.size()));

    for (int i = 0; i < categoryAxis.size(); ++i)
    {
        const auto& discType = categoryAxis.value(i);
        for (int j = 0; j < regularAxis.size(); ++j)
            counts << histogram.at(i, j);

        histogram_.at(discType)->setData(binCenters, counts, true);
        counts.clear();
    }

    setHistogramYRange();
    replot();
}

void PlotWidget::setPlotTitle(const std::string& title)
{
    plotTitle_ = new QCPTextElement(this, QString::fromStdString(title), QFont("sans", 12, QFont::Bold));
}

void PlotWidget::reset()
{
    resetRanges();
    resetGraphs();
    count_ = 0;
}

void PlotWidget::resetRanges()
{
    xMin_ = 0;
    xMax_ = INT_MIN;
    yMin_ = INT_MAX;
    yMax_ = INT_MIN;

    // These seem to be the default values used by QCustomPlot
    yAxis->setRange(0, 5);
    xAxis->setRange(0, 5);
}

void PlotWidget::resetGraphs()
{
    clearGraphs();
    clearPlottables();

    graphs_.clear();
    histogram_.clear();
}

void PlotWidget::updateLegend(const std::vector<std::string>& labels)
{
    if (labels.size() == 1)
        legend->setVisible(false);
    else
    {
        legend->setVisible(true);
        for (int i = 0; i < legend->itemCount(); ++i)
            legend->item(i)->setLayer("legend layer");
    }
}

void PlotWidget::enableZoom(bool enabled)
{
    setInteraction(QCP::iRangeDrag, enabled); // Allow dragging the plot by left click-hold
    setInteraction(QCP::iRangeZoom, enabled); // Allow zoom with mouse wheel
}

void PlotWidget::setHistogramYRange()
{
    double yMax = 0;
    bool foundRange;
    for (int i = 0; i < plottableCount(); ++i)
    {
        QCPRange r = plottable(i)->getValueRange(foundRange);
        if (foundRange)
            yMax = std::max(yMax, r.upper);
    }

    if (yMax > yMax_)
    {
        yMax_ = yMax;
        yAxis->setRange(0, yMax);
    }
    else if (yMax < 0.9 * yMax_)
    {
        yMax_ = yMax;
        yAxis->setRange(0, yMax);
    }
}

void PlotWidget::setModel(PlotModel* plotModel)
{
    connect(plotModel, &PlotModel::createLinePlots, this, &PlotWidget::createLinePlots);
    connect(plotModel, &PlotModel::createHistogram, this, &PlotWidget::createHistogram);
    connect(plotModel, &PlotModel::linePlotPoint, this, &PlotWidget::plotLinePlotPoint);
    connect(plotModel, &PlotModel::linePlotPoints, this, &PlotWidget::plotLinePlotPoints);
    connect(plotModel, &PlotModel::histogram, this, &PlotWidget::plotHistogram);
    connect(plotModel, &PlotModel::plotTitle, this, &PlotWidget::setPlotTitle);
}
