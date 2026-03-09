#include "widgets/PlotWidget.hpp"
#include "cell/ExceptionWithLocation.hpp"
#include "core/Utility.hpp"

#include <QFont>
#include <limits>

PlotWidget::PlotWidget(QWidget* parent)
    : QCustomPlot(parent)
{
    xAxis->setLabel("t [s]");

    addLayer("legend layer");
    legend->setLayer("legend layer");
    legend->setVisible(true);

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

void PlotWidget::createHeatMap(double xMin, double xMax, double yMin, double yMax, int xCells, int yCells)
{
    reset();

    heatMap_ = new QCPColorMap(xAxis, yAxis);
    heatMap_->data()->setSize(std::max(1, xCells), std::max(1, yCells));
    heatMap_->data()->setRange(QCPRange(xMin, xMax), QCPRange(yMin, yMax));
    heatMap_->setInterpolate(true);

    QCPColorGradient gradient;
    gradient.setColorStopAt(0.0, Qt::black);
    gradient.setColorStopAt(1.0, Qt::white);
    heatMap_->setGradient(gradient);

    xAxis->setLabel("t [s]");
    yAxis->setLabel("v.x");
    xAxis->setRange(xMin, xMax);
    yAxis->setRange(yMin, yMax);

    legend->setVisible(false);
    enableZoom(true);

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

    const auto nBins = static_cast<qsizetype>(regularAxis.size());
    QVector<double> counts;
    QVector<double> stackedCounts(nBins, 0.0);
    double maxStackedCount = 0;
    counts.reserve(nBins);

    for (int i = 0; i < categoryAxis.size(); ++i)
    {
        const auto& discType = categoryAxis.value(i);
        for (int j = 0; j < regularAxis.size(); ++j)
        {
            const double count = histogram.at(i, j);
            counts << count;
            stackedCounts[j] += count;
            maxStackedCount = std::max(stackedCounts[j], maxStackedCount);
        }

        histogram_.at(discType)->setData(binCenters, counts, true);
        counts.clear();
    }

    setHistogramYRange(maxStackedCount);
    replot();
}

void PlotWidget::plotHeatMap(const HeatMapData& columns)
{
    if (!heatMap_)
        return;

    const int xCells = std::max(1, static_cast<int>(columns.size()));
    const int yCells = heatMap_->data()->valueSize();

    heatMap_->data()->setSize(xCells, yCells);

    double maxValue = 0.0;

    for (int x = 0; x < columns.size(); ++x)
    {
        const auto& column = columns[x];
        for (int y = 0; y < column.size(); ++y)
        {
            const double value = column[y];
            heatMap_->data()->setCell(x, y, value);
            maxValue = std::max(maxValue, value);
        }
    }

    heatMap_->setDataRange(QCPRange(0.0, std::max(1.0, maxValue)));
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

    yAxis->setRange(0, 5);
    xAxis->setRange(0, 5);
}

void PlotWidget::resetGraphs()
{
    clearGraphs();
    clearPlottables();

    graphs_.clear();
    histogram_.clear();
    heatMap_ = nullptr;
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
    setInteraction(QCP::iRangeDrag, enabled);
    setInteraction(QCP::iRangeZoom, enabled);
}

void PlotWidget::setHistogramYRange(double yMax)
{
    if (yMax > yMax_)
    {
        yMax_ = yMax * 1.2;
        yAxis->setRange(0, yMax_);
    }
    else if (yMax < yMax_ * 0.8)
    {
        yMax_ = yMax;
        yAxis->setRange(0, yMax_);
    }
}

void PlotWidget::setModel(PlotModel* plotModel)
{
    connect(plotModel, &PlotModel::createLinePlots, this, &PlotWidget::createLinePlots);
    connect(plotModel, &PlotModel::createHistogram, this, &PlotWidget::createHistogram);
    connect(plotModel, &PlotModel::createHeatMap, this, &PlotWidget::createHeatMap);

    connect(plotModel, &PlotModel::linePlotPoint, this, &PlotWidget::plotLinePlotPoint);
    connect(plotModel, &PlotModel::linePlotPoints, this, &PlotWidget::plotLinePlotPoints);
    connect(plotModel, &PlotModel::histogram, this, &PlotWidget::plotHistogram);
    connect(plotModel, &PlotModel::heatMap, this, &PlotWidget::plotHeatMap);

    connect(plotModel, &PlotModel::plotTitle, this, &PlotWidget::setPlotTitle);
}