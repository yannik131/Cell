#include "widgets/PlotWidget.hpp"
#include "cell/ExceptionWithLocation.hpp"
#include "core/Utility.hpp"
#include "models/PlotModel.hpp"

PlotWidget::PlotWidget(QWidget* parent)
    : QCustomPlot(parent)
{
    xAxis->setLabel("t [s]");

    addLayer("legend layer");
    legend->setLayer("legend layer");

    // Place the legend
    axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop | Qt::AlignRight);

    plotTimer_.setTimerType(Qt::PreciseTimer);
    plotTimer_.setInterval(1000 / 60);
    connect(&plotTimer_, &QTimer::timeout, this, &PlotWidget::replotIfNewPlotAvailable);
}

void PlotWidget::setModel(PlotModel* plotModel)
{
    connect(plotModel, qOverload<const LinePlotParams&>(&PlotModel::setPlot), this,
            qOverload<const LinePlotParams&>(&PlotWidget::setPlot));
    connect(plotModel, qOverload<const HistogramParams&>(&PlotModel::setPlot), this,
            qOverload<const HistogramParams&>(&PlotWidget::setPlot));
    connect(plotModel, qOverload<const ColorMapParams&>(&PlotModel::setPlot), this,
            qOverload<const ColorMapParams&>(&PlotWidget::setPlot));
    connect(plotModel, qOverload<const LinePlotData&>(&PlotModel::updatePlot), this,
            qOverload<const LinePlotData&>(&PlotWidget::updatePlot));
    connect(plotModel, qOverload<const HistogramData&>(&PlotModel::updatePlot), this,
            qOverload<const HistogramData&>(&PlotWidget::updatePlot));
    connect(plotModel, qOverload<const ColorMapData&>(&PlotModel::updatePlot), this,
            qOverload<const ColorMapData&>(&PlotWidget::updatePlot));

    connect(plotModel, &PlotModel::interpolateEnabled, this, &PlotWidget::setInterpolate);

    discTypeRegistryProvider_ = [plotModel]() -> const cell::DiscTypeRegistry&
    { return plotModel->getDiscTypeRegistry(); };
    plotSumProvider_ = [plotModel]() -> bool { return plotModel->plotSum(); };
}

void PlotWidget::setPlot(const LinePlotParams& linePlotParams)
{
    const auto& labels = linePlotParams.labels;
    const auto& colors = linePlotParams.colors;

    clear();

    xAxis->setLabel("t [s]");
    yAxis->setLabel(getYAxisLabelFromPlotCategory(linePlotParams.plotCategory));

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

    for (std::size_t i = 0; i < linePlotParams.dataPoints.size(); ++i)
        updatePlot(
            LinePlotData{.dataPoint = linePlotParams.dataPoints[i], .doReplot = false, .x = linePlotParams.x[i]});

    QCustomPlot::replot();
}

void PlotWidget::setPlot(const HistogramParams& histogramParams)
{
    const auto& labels = histogramParams.labels;
    const auto& colors = histogramParams.colors;

    clear();

    if (labels.size() != colors.size())
        throw ExceptionWithLocation("Must have equal number of labels and colors");

    const auto& ax = histogramParams.histogram.axis(1);
    const double binWidth = ax.bin(0).width();

    for (std::size_t i = 0; i < labels.size(); ++i)
    {
        auto* graph = new QCPBars(xAxis, yAxis);
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

    updatePlot(HistogramData{.histogram = histogramParams.histogram});
    QCustomPlot::replot();
}

void PlotWidget::setPlot(const ColorMapParams& colorMapParams)
{
    const auto xStep = colorMapParams.xStep;
    const auto& histograms = colorMapParams.histograms;

    clear();
    if (histograms.empty())
        throw ExceptionWithLocation("Can't initialize plot with empty histograms");

    legend->setVisible(false);

    colorMap_ = new QCPColorMap(xAxis, yAxis);
    colorScale_ = new QCPColorScale(this);
    colorScale_->setType(QCPAxis::atRight);
    QCustomPlot::plotLayout()->addElement(0, 1, colorScale_);
    colorMap_->setColorScale(colorScale_);
    colorMap_->setInterpolate(interpolateEnabled_);

    const int binCount = histograms.front().axis(1).size();
    colorMap_->data()->setSize(static_cast<int>(histograms.size()), binCount);

    xAxis->setLabel("t [s]");
    xAxis->setRange(0, xStep * static_cast<double>(histograms.size()));
    yAxis->setRange(0, binCount);
    if (histograms.size() == 1)
        colorMap_->data()->setRange(QCPRange(0, xStep), QCPRange(0.5, binCount - 0.5));
    else
        colorMap_->data()->setRange(QCPRange(xStep / 2, xStep * static_cast<double>(histograms.size()) - xStep / 2),
                                    QCPRange(0.5, binCount - 0.5));

    for (int x = 0; x < static_cast<int>(histograms.size()); ++x)
    {
        for (int y = 0; y < binCount; ++y)
            colorMap_->data()->setCell(x, y, histograms[x].at(0, y));
    }
    colorMapCache_ = histograms;

    colorMap_->rescaleDataRange();

    colorMap_->setGradient(QCPColorGradient::gpGrayscale);

    auto ticker = QSharedPointer<QCPAxisTickerText>::create();
    for (int i = 0; i < binCount; ++i)
        ticker->addTick(i + 0.5, QString::number(histograms.front().axis(1).bin(i).center()));
    yAxis->setTicker(ticker);
    yAxis->setLabel(getYAxisLabelFromPlotCategory(PlotCategory::VelocityColorMap));
    QCustomPlot::replot();
}

void PlotWidget::updatePlot(const LinePlotData& linePlotData)
{
    const auto& discTypeRegistry = getDiscTypeRegistry();
    const auto& dataPoint = linePlotData.dataPoint;
    const bool plotSum = plotSumProvider_();

    for (auto& [label, graph] : graphs_)
    {
        double value = 0;
        const auto& discTypeID = plotSum ? 0 : discTypeRegistry.getIDFor(label);
        const auto& iter = dataPoint.find(discTypeID);
        if (iter != dataPoint.end())
            value = iter->second;

        graph->addData(linePlotData.x, value);

        yMin_ = std::min(yMin_, value);
        yMax_ = std::max(yMax_, value);
    }

    yAxis->setRange(yMin_ - 1, yMax_ + 1);
    xAxis->setRange(xMin_, linePlotData.x);
    newPlotAvailable_ = true;
}

void PlotWidget::updatePlot(const HistogramData& histogramData)
{
    const auto& histogram = histogramData.histogram;
    const auto& categoryAxis = histogram.axis<0>();
    const auto& regularAxis = histogram.axis<1>();
    const auto& discTypeRegistry = getDiscTypeRegistry();
    const bool plotSum = plotSumProvider_();

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

        histogram_.at(plotSum ? "Sum" : discTypeRegistry.getByID(discType).getName())
            ->setData(binCenters, counts, true);
        counts.clear();
    }

    setHistogramYRange(maxStackedCount);
    newPlotAvailable_ = true;
}

void PlotWidget::updatePlot(const ColorMapData& colorMapData)
{
    const double xStep = colorMapData.xStep;
    colorMapCache_.push_back(colorMapData.histogram);
    const int binCount = colorMap_->data()->valueSize();
    colorMap_->data()->setSize(static_cast<int>(colorMapCache_.size()), binCount);

    for (int x = 0; x < static_cast<int>(colorMapCache_.size()); ++x)
    {
        for (int y = 0; y < binCount; ++y)
            colorMap_->data()->setCell(x, y, colorMapCache_[x].at(0, y));
    }

    colorMap_->rescaleDataRange();
    xAxis->setRange(0, xStep * static_cast<double>(colorMapCache_.size()));
    colorMap_->data()->setRange(QCPRange(xStep / 2, xStep * static_cast<double>(colorMapCache_.size()) - xStep / 2),
                                QCPRange(0.5, binCount - 0.5));
    newPlotAvailable_ = true;
}

void PlotWidget::clear()
{
    clearRanges();
    clearGraphs();
    colorMapCache_.clear();
}

void PlotWidget::clearRanges()
{
    xMin_ = 0;
    xMax_ = std::numeric_limits<double>::lowest();
    yMin_ = std::numeric_limits<double>::max();
    yMax_ = std::numeric_limits<double>::lowest();

    yAxis->setTicker(QSharedPointer<QCPAxisTicker>::create());

    // These seem to be the default values used by QCustomPlot
    yAxis->setRange(0, 5);
    xAxis->setRange(0, 5);

    xAxis->setNumberPrecision(2);
}

void PlotWidget::clearGraphs()
{
    QCustomPlot::clearGraphs();
    QCustomPlot::clearPlottables();

    graphs_.clear();
    histogram_.clear();

    if (colorScale_)
    {
        QCustomPlot::plotLayout()->remove(colorScale_);
        colorScale_ = nullptr;
        colorMap_ = nullptr;
        QCustomPlot::plotLayout()->simplify();
    }
}

void PlotWidget::updateLegend(const std::vector<std::string>& labels)
{
    if (labels.size() <= 1)
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

QString PlotWidget::getYAxisLabelFromPlotCategory(const PlotCategory& plotCategory) const
{
    switch (plotCategory)
    {
    case PlotCategory::AbsoluteMomentum: return "|mv|";
    case PlotCategory::CollisionCounts: return "Collision count";
    case PlotCategory::KineticEnergy: return "E_kin";
    case PlotCategory::TypeCounts: return "Number of discs";
    case PlotCategory::VelocityColorMap: return "v_x";
    case PlotCategory::VelocityDistribution: return "Count";
    default: throw ExceptionWithLocation("Invalid plot category");
    }
}

void PlotWidget::replotIfNewPlotAvailable()
{
    if (!newPlotAvailable_)
        return;

    QCustomPlot::replot();
    newPlotAvailable_ = false;
}

void PlotWidget::setInterpolate(bool enabled)
{
    interpolateEnabled_ = enabled;
    if (!colorMap_)
        return;

    colorMap_->setInterpolate(enabled);
    replot(QCustomPlot::rpQueuedReplot);
}

const cell::DiscTypeRegistry& PlotWidget::getDiscTypeRegistry() const
{
    return discTypeRegistryProvider_();
}

void PlotWidget::startPlotTimer()
{
    plotTimer_.start();
    enableZoom(false);
}

void PlotWidget::stopPlotTimer()
{
    plotTimer_.stop();
    enableZoom(true);
}
