#include "widgets/PlotWidget.hpp"
#include "cell/ExceptionWithLocation.hpp"
#include "core/Utility.hpp"

#include <QFont>

PlotWidget::PlotWidget(QWidget* parent)
    : QCustomPlot(parent)
{
    setInteraction(QCP::iRangeDrag, true); // Allow dragging the plot by left click-hold
    setInteraction(QCP::iRangeZoom, true); // Allow zoom with mouse wheel

    xAxis->setLabel("t [s]");

    addLayer("legend layer");
    legend->setLayer("legend layer");
    legend->setVisible(true);

    // Place the legend
    axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop | Qt::AlignCenter);
}

void PlotWidget::createGraphs(const std::vector<std::string>& labels, const std::vector<sf::Color>& colors)
{
    reset();

    if (labels.size() != colors.size())
        throw std::logic_error("Must have equal number of labels and colors");

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

    if (labels.size() == 1)
        legend->setVisible(false);
    else
    {
        legend->setVisible(true);
        for (int i = 0; i < legend->itemCount(); ++i)
            legend->item(i)->setLayer("legend layer");
    }

    replot();
}

void PlotWidget::addDataPoint(const std::unordered_map<std::string, double>& dataPoint, double xStep, DoReplot doReplot)
{
    double x = xStep * count_++;

    for (const auto& [label, value] : dataPoint)
    {
        auto graph = graphs_.find(label);
        if (graph == graphs_.end())
            throw std::logic_error("Invalid datapoint: There is no plot with label\"" + label + "\"");

        graph->second->addData(x, value);

        yMin_ = std::min(yMin_, value);
        yMax_ = std::max(yMax_, value);
    }

    yAxis->setRange(yMin_ - 1, yMax_ + 1);
    xAxis->setRange(xMin_, x);

    if (doReplot.value)
        replot();
}

void PlotWidget::addDataPoints(const std::vector<std::unordered_map<std::string, double>>& dataPoints, double xStep)
{
    for (std::size_t i = 0; i < dataPoints.size(); ++i)
        addDataPoint(dataPoints[i], xStep, DoReplot{false});

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

    setAxisLabels();
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
    graphs_.clear();
}

void PlotWidget::setAxisLabels()
{
}

void PlotWidget::setModel(PlotModel* plotModel)
{
    connect(plotModel, &PlotModel::createGraphs, this, &PlotWidget::createGraphs);
    connect(plotModel, &PlotModel::addDataPoint, this, &PlotWidget::addDataPoint);
    connect(plotModel, &PlotModel::addDataPoints, this, &PlotWidget::addDataPoints);
    connect(plotModel, &PlotModel::setPlotTitle, this, &PlotWidget::setPlotTitle);
}
