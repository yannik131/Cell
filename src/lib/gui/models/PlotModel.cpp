#include "models/PlotModel.hpp"
#include "cell/MathUtils.hpp"
#include "core/Simulation.hpp"
#include "core/Utility.hpp"

#include "PlotModel.hpp"
#include <cmath>
#include <unordered_set>

using DataPoint = cell::DataPoint;

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

    plotTimeInterval_ = static_cast<double>(valueMilliseconds) / 1000.0;

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
}

void PlotModel::setActivePlotDiscTypes(const std::vector<std::string>& activeDiscTypeNames)
{
    activePlotDiscTypes_.clear();
    for (const auto& name : activeDiscTypeNames)
        activePlotDiscTypes_[name] = true;

    setPlot();
}

const std::map<std::string, bool>& PlotModel::getActivePlotDiscTypesMap() const
{
    return activePlotDiscTypes_;
}

void PlotModel::setPlot()
{
    updateLabelsAndColors();

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
    std::vector<std::unordered_map<std::string, double>> fullPlotData;
    const auto& dataPoints = simulation_->getSimulationRecorder().getDataPoints();
    int averagingCount = 0;
    DataPoint dataPoint;

    for (const auto& p : dataPoints)
    {
        // TODO This adds everything, but we only need the current plot category
        dataPoint.add(p);
        ++averagingCount;

        if (dataPoint.elapsedTime_ < plotTimeInterval_)
            continue;

        dataPoint.average(averagingCount);
        const auto& activeMap = getActiveMap(dataPoint);

        if (plotSum_)
        {
            auto sum = std::accumulate(activeMap.begin(), activeMap.end(), 0.0,
                                       [](double partialSum, const auto& pair) { return pair.second + partialSum; });
            fullPlotData.push_back({{"Sum", sum}});
        }
        else
            fullPlotData.push_back(getActiveMap(dataPoint));

        dataPoint.clear();
        averagingCount = 0;
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

    if (dataPoints.empty())
        return;

    if (static_cast<int>(dataPoints.size()) < requiredDataPoints)
        histogram = getVelocityHistogramFromDataPoint(dataPoints.front(), CalculateSum{plotSum_});
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
    const auto& dataPoints = simulation_->getSimulationRecorder().getDataPoints();
    histograms.reserve(dataPoints.size());

    DataPoint dataPoint;
    int averagingCount = 0;

    for (const auto& p : dataPoints)
    {
        dataPoint.add(p);
        ++averagingCount;

        if (dataPoint.elapsedTime_ < plotTimeInterval_)
            continue;

        dataPoint.average(averagingCount);
        histograms.push_back(getVelocityHistogramFromDataPoint(dataPoint, CalculateSum{true}));

        dataPoint.clear();
        averagingCount = 0;
    }

    // Simulation hasn't run yet, display plot for initial data
    if (histograms.empty() && !dataPoints.empty())
        histograms.push_back(getVelocityHistogramFromDataPoint(dataPoints.front(), CalculateSum{true}));

    emit setPlot(PlotWidget::ColorMapParams{.histograms = histograms, .xStep = plotTimeInterval_.count()});
}

void PlotModel::updatePlot(const cell::DataPoint& dataPoint)
{
    switch (plotCategory_)
    {
    case PlotCategory::TypeCounts:
    case PlotCategory::AbsoluteMomentum:
    case PlotCategory::CollisionCounts:
    case PlotCategory::KineticEnergy: updateLinePlot(dataPoint); break;
    case PlotCategory::VelocityDistribution: updateHistogramPlot(dataPoint); break;
    case PlotCategory::VelocityColorMap: updateColorMapPlot(dataPoint); break;
    default: throw ExceptionWithLocation("Invalid plot category");
    }
}

void PlotModel::updateLinePlot(const cell::DataPoint& dataPoint)
{
    const auto& activeMap = getActiveMap(dataPoint);

    if (plotSum_)
    {
        auto sum = std::accumulate(activeMap.begin(), activeMap.end(), 0.0,
                                   [&](double partialSum, const auto& pair) { return pair.second + partialSum; });
        emit updatePlot(PlotWidget::LinePlotData{.dataPoint = {{"Sum", sum}}, .doReplot = true});
    }
    else
        emit updatePlot(PlotWidget::LinePlotData{.dataPoint = activeMap, .doReplot = true});
}

void PlotModel::updateHistogramPlot(const cell::DataPoint& dataPoint)
{
    auto histogram = getVelocityHistogramFromDataPoint(dataPoint, CalculateSum{plotSum_});
    emit updatePlot(PlotWidget::HistogramData{.histogram = histogram});
}

void PlotModel::updateColorMapPlot(const cell::DataPoint& dataPoint)
{
    auto histogram = getVelocityHistogramFromDataPoint(dataPoint, CalculateSum{true});
    emit updatePlot(PlotWidget::ColorMapData{.histogram = histogram});
}

void PlotModel::updateLabelsAndColors()
{
    labels_.clear();
    colors_.clear();

    const auto& config = simulation_->getSimulationConfig();
    const auto& colorMap = simulation_->getSimulationConfigUpdater().getDiscTypeColorMap();

    if (plotSum_)
    {
        labels_.emplace_back("Sum");
        colors_.push_back(sf::Color::Black);
    }
    else
    {
        for (const auto& discType : config.discTypes)
        {
            if (!activePlotDiscTypes_[discType.name])
                continue;

            labels_.push_back(discType.name);
            colors_.push_back(colorMap.at(discType.name));
        }
    }
}

std::unordered_map<DiscTypeID, double> PlotModel::getActiveMap(const DataPoint& dataPoint)
{
    std::unordered_map<DiscTypeID, double> activeMap;

    switch (plotCategory_)
    {
    case PlotCategory::TypeCounts: activeMap = dataPoint.discTypeCounts_; break;
    case PlotCategory::CollisionCounts: activeMap = dataPoint.collisionCounts_; break;
    case PlotCategory::AbsoluteMomentum: activeMap = dataPoint.totalMomentums_; break;
    case PlotCategory::KineticEnergy: activeMap = dataPoint.totalKineticEnergies_; break;
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

void PlotModel::storeDataPoint(const DataPoint& dataPoint)
{
    dataPointForStorage_ += dataPoint;
    const double timeStep = simulation_->getSimulationConfig().simulationTimeStep;

    if (dataPointForStorage_.elapsedTime_ >= storageTime_)
    {
        const int dataPointsPerStoredPoint = static_cast<int>(std::ceil(storageTime_ / timeStep));
        averageDataPoint(dataPointForStorage_, dataPointsPerStoredPoint, NormalizeCollisionCounts{false});
        dataPoints.push_back(dataPointForStorage_);
        dataPointForStorage_.clear();
    }

    // TODO The plot widget assumes all data points are equidistant (constant xStep), but data points can have varying
    // elapsed time, so dt should be calculated as the distance from the last data point and sent to the plot widget as
    // well. Example: simulation time step = 40ms, plot time interval = 100 ms => we collect 3 datapoints until elapsed
    // time >= plot time interval (120 ms >= 100 ms), so each dataPointForPlotting_ spans 120ms (same for storage if
    // storage time = 100ms)

    dataPointForPlotting_ += dataPoint;

    if (dataPointForPlotting_.elapsedTime_ >= plotTimeInterval_)
    {
        const int N = static_cast<int>(std::round(dataPointForPlotting_.elapsedTime_ / timeStep));
        averageDataPoint(dataPointForPlotting_, N);
        updatePlot();

        dataPointForPlotting_.clear();
    }
}

void PlotModel::updateActivePlotDiscTypes(const std::vector<cell::config::DiscType>& discTypes)
{
    std::unordered_set<std::string> discTypeNames;

    // Make all new disc types active by default
    for (const auto& discType : discTypes)
    {
        if (!activePlotDiscTypes_.contains(discType.name))
            activePlotDiscTypes_[discType.name] = true;

        discTypeNames.insert(discType.name);
    }

    // Remove disc types that were deleted
    for (auto iter = activePlotDiscTypes_.begin(); iter != activePlotDiscTypes_.end();)
    {
        if (!discTypeNames.contains(iter->first))
            iter = activePlotDiscTypes_.erase(iter);
        else
            ++iter;
    }
}

cell::Histogram PlotModel::sumHistogramStacks(const cell::Histogram& histogram)
{
    const auto& categoryAxis = histogram.axis<0>();
    const auto& regularAxis = histogram.axis<1>();
    cell::Histogram sumcell::Histogram = makeHistogramWithCategories(histogram, {"Sum"});

    for (int i = 0; i < regularAxis.size(); ++i)
    {
        double sum = 0.0;
        for (int j = 0; j < categoryAxis.size(); ++j)
            sum += histogram.at(j, i);

        sumHistogram.at(0, i) = sum;
    }

    return sumHistogram;
}

cell::Histogram PlotModel::discardInactiveDiscTypes(const cell::Histogram& histogram)
{
    const auto& categoryAxis = histogram.axis<0>();
    const auto& regularAxis = histogram.axis<1>();
    std::vector<std::string> activeDiscTypes;
    for (int i = 0; i < categoryAxis.size(); ++i)
    {
        const auto& discType = categoryAxis.value(i);
        if (activePlotDiscTypes_[discType])
            activeDiscTypes.push_back(discType);
    }

    cell::Histogram filteredcell::Histogram = makeHistogramWithCategories(histogram, activeDiscTypes);

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

cell::Histogram PlotModel::getVelocityHistogramFromDataPoint(const DataPoint& dataPoint, CalculateSum calculateSum)
{
    auto h = discardInactiveDiscTypes(dataPoint.vxHistogram_);
    if (calculateSum.value)
        h = sumHistogramStacks(h);

    return h;
}

cell::Histogram PlotModel::makeHistogramWithCategories(const cell::Histogram& source,
                                                       const std::vector<std::string>& categories)
{
    const auto& regularAxis = source.axis<1>();

    return bh::make_histogram(bh::axis::category<std::string>(categories, "Disc types"),
                              bh::axis::regular<>(regularAxis.size(), regularAxis.value(0),
                                                  regularAxis.value(regularAxis.size()), regularAxis.metadata()));
}

DataPoint& operator+=(DataPoint& lhs, const DataPoint& rhs)
{
    lhs.elapsedTime_ += rhs.elapsedTime_;

    utility::addMaps(lhs.collisionCounts_, rhs.collisionCounts_);
    utility::addMaps(lhs.discTypeCountMap_, rhs.discTypeCountMap_);
    utility::addMaps(lhs.totalKineticEnergyMap_, rhs.totalKineticEnergyMap_);
    utility::addMaps(lhs.totalMomentumMap_, rhs.totalMomentumMap_);
    lhs.vxHistogram_ += rhs.vxHistogram_;

    return lhs;
}
