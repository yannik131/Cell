#include "models/PlotModel.hpp"
#include "cell/MathUtils.hpp"
#include "cell/SimulationContext.hpp"
#include "core/Simulation.hpp"
#include "core/Utility.hpp"

#include "PlotModel.hpp"
#include <cmath>
#include <unordered_set>

PlotModel::PlotModel(QObject* parent, Simulation* simulation)
    : QObject(parent)
    , simulation_(simulation)
{
}

void PlotModel::setPlotCategory(PlotCategory plotCategory)
{
    static const std::unordered_map<PlotCategory, int> graphType{
        {PlotCategory::AbsoluteMomentum, 1},     {PlotCategory::CollisionCounts, 1},
        {PlotCategory::KineticEnergy, 1},        {PlotCategory::TypeCounts, 1},
        {PlotCategory::VelocityDistribution, 2}, {PlotCategory::VelocityColorMap, 3}};

    plotCategory_ = plotCategory;
    setPlot();
}

void PlotModel::setPlotTimeInterval(int valueMilliseconds)
{
    // To reduce the number of datapoints, we store 100ms intervals
    if (valueMilliseconds < 100)
        throw ExceptionWithLocation("The plot time interval must be at least 100ms");

    if (valueMilliseconds % 100 != 0)
        throw ExceptionWithLocation("The plot time interval must be a multiple of 100ms");

    plotTimeInterval_ = ch::duration<double>{static_cast<double>(valueMilliseconds) / 1000.0};

    setPlot();
}

void PlotModel::setPlotSum(bool value)
{
    plotSum_ = value;
    setPlot();
}

void PlotModel::setInterpolate(bool value)
{
    emit interpolateEnabled(value);
}

void PlotModel::reset()
{
    updateActivePlotDiscTypes(simulation_->getSimulationConfig().discTypes);
    setPlot();
}

void PlotModel::processDataPoint(const cell::DataPoint& dataPoint)
{
    updatePlot(dataPoint);
}

void PlotModel::setActivePlotDiscTypes(const std::vector<std::string>& activeDiscTypeNames)
{
    const auto& discTypeRegistry = getDiscTypeRegistry();
    activePlotDiscTypes_.clear();
    for (const auto& name : activeDiscTypeNames)
        activePlotDiscTypes_[discTypeRegistry.getIDFor(name)] = true;

    setPlot();
}

bool PlotModel::discTypeIsActiveInPlot(const std::string& discTypeName) const
{
    const auto& discTypeRegistry = getDiscTypeRegistry();
    return activePlotDiscTypes_.at(discTypeRegistry.getIDFor(discTypeName));
}

void PlotModel::setPlot()
{
    updateLabelsAndColors();
    dataPoint_ = createDataPoint();

    switch (plotCategory_)
    {
    case PlotCategory::TypeCounts:
    case PlotCategory::AbsoluteMomentum:
    case PlotCategory::CollisionCounts:
    case PlotCategory::KineticEnergy: setLinePlot(); break;
    case PlotCategory::VelocityDistribution: setHistogramPlot(); break;
    case PlotCategory::VelocityColorMap: setColorMapPlot(); break;
    default: throw ExceptionWithLocation("Invalid plot category");
    }
}

void PlotModel::setLinePlot()
{
    std::vector<std::unordered_map<DiscTypeID, double>> fullPlotData;
    const auto& dataPoints = simulation_->getSimulationRecorder().getDataPoints();
    auto dataPoint = createDataPoint();

    for (const auto& p : dataPoints)
    {
        // TODO This adds everything, but we only need the current plot category
        dataPoint.add(p);

        if (dataPoint.getData().elapsedTime < plotTimeInterval_)
            continue;

        dataPoint.average(cell::NormalizeCollisionCounts{true});
        const auto& activeMap = getActiveMap(dataPoint);

        if (plotSum_)
        {
            auto sum = std::accumulate(activeMap.begin(), activeMap.end(), 0.0,
                                       [](double partialSum, const auto& pair) { return pair.second + partialSum; });
            fullPlotData.push_back({{0, sum}});
        }
        else
            fullPlotData.push_back(getActiveMap(dataPoint));

        dataPoint.clear();
    }

    emit setPlot(PlotWidget::LinePlotParams{.labels = labels_,
                                            .colors = colors_,
                                            .dataPoints = fullPlotData,
                                            .plotCategory = plotCategory_,
                                            .xStep = plotTimeInterval_.count()});
}

void PlotModel::setHistogramPlot()
{
    cell::Histogram histogram;
    const auto storageTime = simulation_->getSimulationRecorder().getStorageInterval();
    const auto& dataPoints = simulation_->getSimulationRecorder().getDataPoints();
    const int requiredDataPoints = std::max(1, static_cast<int>(std::ceil(plotTimeInterval_ / storageTime)));

    if (static_cast<int>(dataPoints.size()) < requiredDataPoints)
    {
        const auto& dataPoint = simulation_->getSimulationRecorder().getCurrentDataPoint();
        histogram = getVelocityHistogramFromDataPoint(dataPoint, CalculateSum{plotSum_});
    }
    else
    {
        histogram = getVelocityHistogramFromDataPoint(dataPoints.back(), CalculateSum{plotSum_});

        for (int i = 1; i < requiredDataPoints; ++i)
            histogram += getVelocityHistogramFromDataPoint(dataPoints[i], CalculateSum{plotSum_});

        histogram /= requiredDataPoints;
    }

    emit setPlot(PlotWidget::HistogramParams{.labels = labels_, .colors = colors_, .histogram = histogram});
}

void PlotModel::setColorMapPlot()
{
    std::vector<cell::Histogram> histograms;
    const auto& simulationRecorder = simulation_->getSimulationRecorder();
    const auto& dataPoints = simulationRecorder.getDataPoints().empty()
                                 ? std::vector<DataPoint>({simulationRecorder.getCurrentDataPoint()})
                                 : simulationRecorder.getDataPoints();
    histograms.reserve(dataPoints.size());

    auto dataPoint = createDataPoint();

    for (const auto& p : dataPoints)
    {
        dataPoint.add(p);

        if (dataPoint.getData().elapsedTime < plotTimeInterval_)
            continue;

        dataPoint.average();
        histograms.push_back(getVelocityHistogramFromDataPoint(dataPoint, CalculateSum{true}));
        dataPoint.clear();
    }

    // Simulation hasn't run yet, display plot for initial data
    if (histograms.empty() && !dataPoints.empty())
        histograms.push_back(getVelocityHistogramFromDataPoint(dataPoints.front(), CalculateSum{true}));

    emit setPlot(PlotWidget::ColorMapParams{.histograms = histograms, .xStep = plotTimeInterval_.count()});
}

void PlotModel::updatePlot(const DataPoint& dataPoint)
{
    dataPoint_.add(dataPoint);

    if (dataPoint_.getData().elapsedTime < plotTimeInterval_)
        return;

    dataPoint_.average(cell::NormalizeCollisionCounts{true});

    switch (plotCategory_)
    {
    case PlotCategory::TypeCounts:
    case PlotCategory::AbsoluteMomentum:
    case PlotCategory::CollisionCounts:
    case PlotCategory::KineticEnergy: updateLinePlot(); break;
    case PlotCategory::VelocityDistribution: updateHistogramPlot(); break;
    case PlotCategory::VelocityColorMap: updateColorMapPlot(); break;
    default: throw ExceptionWithLocation("Invalid plot category");
    }

    dataPoint_.clear();
}

void PlotModel::updateLinePlot()
{
    const auto& activeMap = getActiveMap(dataPoint_);

    if (plotSum_)
    {
        auto sum = std::accumulate(activeMap.begin(), activeMap.end(), 0.0,
                                   [&](double partialSum, const auto& pair) { return pair.second + partialSum; });
        emit updatePlot(PlotWidget::LinePlotData{.dataPoint = {{0, sum}}, .doReplot = true});
    }
    else
        emit updatePlot(PlotWidget::LinePlotData{.dataPoint = activeMap, .doReplot = true});
}

void PlotModel::updateHistogramPlot()
{
    auto histogram = getVelocityHistogramFromDataPoint(dataPoint_, CalculateSum{plotSum_});
    emit updatePlot(PlotWidget::HistogramData{.histogram = histogram});
}

void PlotModel::updateColorMapPlot()
{
    auto histogram = getVelocityHistogramFromDataPoint(dataPoint_, CalculateSum{true});
    emit updatePlot(PlotWidget::ColorMapData{.histogram = histogram});
}

void PlotModel::updateLabelsAndColors()
{
    labels_.clear();
    colors_.clear();

    const auto& config = simulation_->getSimulationConfig();
    const auto& colorMap = simulation_->getSimulationConfigUpdater().getDiscTypeColorMap();
    const auto& discTypeRegistry = getDiscTypeRegistry();

    if (plotSum_)
    {
        labels_.emplace_back("Sum");
        colors_.push_back(sf::Color::Black);
    }
    else
    {
        for (const auto& discType : config.discTypes)
        {
            if (!activePlotDiscTypes_[discTypeRegistry.getIDFor(discType.name)])
                continue;

            labels_.push_back(discType.name);
            colors_.push_back(colorMap.at(discType.name));
        }
    }
}

std::unordered_map<DiscTypeID, double> PlotModel::getActiveMap(const DataPoint& dataPoint)
{
    std::unordered_map<DiscTypeID, double> activeMap;
    const auto& data = dataPoint.getData();

    switch (plotCategory_)
    {
    case PlotCategory::TypeCounts: activeMap = data.discTypeCounts; break;
    case PlotCategory::CollisionCounts: activeMap = data.collisionCounts; break;
    case PlotCategory::AbsoluteMomentum: activeMap = data.totalMomentums; break;
    case PlotCategory::KineticEnergy: activeMap = data.totalKineticEnergies; break;
    default: activeMap = {};
    }

    for (auto iter = activeMap.begin(); iter != activeMap.end();)
    {
        if (!activePlotDiscTypes_[iter->first])
            iter = activeMap.erase(iter);
        else
            ++iter;
    }

    return activeMap;
}

void PlotModel::updateActivePlotDiscTypes(const std::vector<cell::config::DiscType>& discTypes)
{
    std::unordered_set<DiscTypeID> discTypeIDs;
    const auto& discTypeRegistry = getDiscTypeRegistry();

    // Make all new disc types active by default
    for (const auto& discType : discTypes)
    {
        const auto& discTypeID = discTypeRegistry.getIDFor(discType.name);
        if (!activePlotDiscTypes_.contains(discTypeID))
            activePlotDiscTypes_[discTypeID] = true;

        discTypeIDs.insert(discTypeID);
    }

    // Remove disc types that were deleted
    for (auto iter = activePlotDiscTypes_.begin(); iter != activePlotDiscTypes_.end();)
    {
        if (!discTypeIDs.contains(iter->first))
            iter = activePlotDiscTypes_.erase(iter);
        else
            ++iter;
    }
}

Histogram PlotModel::sumHistogramStacks(const Histogram& histogram)
{
    const auto& categoryAxis = histogram.axis<0>();
    const auto& regularAxis = histogram.axis<1>();
    Histogram sumHistogram = makeHistogramWithCategories(histogram, {0});

    for (int i = 0; i < regularAxis.size(); ++i)
    {
        double sum = 0.0;
        for (int j = 0; j < categoryAxis.size(); ++j)
            sum += histogram.at(j, i);

        sumHistogram.at(0, i) = sum;
    }

    return sumHistogram;
}

Histogram PlotModel::discardInactiveDiscTypes(const Histogram& histogram)
{
    const auto& categoryAxis = histogram.axis<0>();
    const auto& regularAxis = histogram.axis<1>();
    std::vector<DiscTypeID> activeDiscTypeIDs;

    for (int i = 0; i < categoryAxis.size(); ++i)
    {
        const auto& discTypeID = categoryAxis.value(i);
        if (activePlotDiscTypes_[discTypeID])
            activeDiscTypeIDs.push_back(discTypeID);
    }

    Histogram filteredHistogram = makeHistogramWithCategories(histogram, activeDiscTypeIDs);

    int category = 0;
    for (int i = 0; i < categoryAxis.size(); ++i)
    {
        const auto& discType = categoryAxis.value(i);
        if (!activePlotDiscTypes_[discType])
            continue;

        for (int j = 0; j < regularAxis.size(); ++j)
            filteredHistogram.at(category, j) = histogram.at(i, j);

        ++category;
    }

    return filteredHistogram;
}

Histogram PlotModel::getVelocityHistogramFromDataPoint(const DataPoint& dataPoint, CalculateSum calculateSum)
{
    auto h = discardInactiveDiscTypes(dataPoint.getData().vxHistogram);
    if (calculateSum.value)
        h = sumHistogramStacks(h);

    return h;
}

Histogram PlotModel::makeHistogramWithCategories(const Histogram& source, const std::vector<DiscTypeID>& categories)
{
    const auto& regularAxis = source.axis<1>();

    return boost::histogram::make_histogram(bh::axis::category<DiscTypeID>(categories, "Disc types"),
                                            bh::axis::regular<>(regularAxis.size(), regularAxis.value(0),
                                                                regularAxis.value(regularAxis.size()),
                                                                regularAxis.metadata()));
}

DataPoint PlotModel::createDataPoint() const
{
    const auto& discTypeRegistry = getDiscTypeRegistry();
    double vSigma = simulation_->getSimulationConfig().mostProbableSpeed;

    DataPoint dataPoint;
    dataPoint.initializeHistograms(discTypeRegistry.getIDs(), vSigma);

    return dataPoint;
}

const cell::DiscTypeRegistry& PlotModel::getDiscTypeRegistry() const
{
    return simulation_->getSimulationContext().discTypeRegistry;
}

bool PlotModel::plotSum() const
{
    return plotSum_;
}
