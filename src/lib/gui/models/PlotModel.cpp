#include "models/PlotModel.hpp"
#include "cell/MathUtils.hpp"
#include "core/Simulation.hpp"
#include "core/Utility.hpp"

#include "PlotModel.hpp"
#include <cmath>
#include <unordered_set>

namespace
{

struct NormalizeCollisionCounts
{
    bool value = true;
};

void averageDataPoint(DataPoint& dataPoint, int length, NormalizeCollisionCounts normalizeCollisionCounts = {})
{
    double dt = dataPoint.elapsedTime_;

    if (normalizeCollisionCounts.value)
    {
        // Collisions per second = All registered collisions / dt
        utility::divideValuesBy(dataPoint.collisionCounts_, dt);
    }

    utility::divideValuesBy(dataPoint.totalKineticEnergyMap_, length);
    utility::divideValuesBy(dataPoint.totalMomentumMap_, length);
    utility::divideValuesBy(dataPoint.discTypeCountMap_, length);
    dataPoint.vxHistogram_ /= length;
}

} // namespace

PlotModel::PlotModel(QObject* parent, Simulation* simulation)
    : QObject(parent)
    , simulation_(simulation)
    , dataPointForStorage_(DataPoint(simulation->getSimulationConfig()))
    , dataPointForPlotting_(DataPoint(simulation->getSimulationConfig()))
{
    // With a simulation time step of 1ms, we get 1000 data points each second
    // With an averaging time of 100ms, we save 10 datapoints for 1 second
    // We'll reserve enough space for 5 minutes of plotting, 5*60*10
    dataPoints_.reserve(3000);

    // Keep histogram axes up to date
    connect(&simulation->getSimulationConfigUpdater(), &SimulationConfigUpdater::simulationResetRequired,
            [&]()
            {
                dataPointForStorage_ = DataPoint(simulation_->getSimulationConfig());
                dataPointForPlotting_ = DataPoint(simulation_->getSimulationConfig());
            });
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

void PlotModel::reset()
{
    dataPoints_.clear();
    dataPointForStorage_.clear();
    dataPointForPlotting_.clear();

    updateActivePlotDiscTypes(simulation_->getSimulationConfig().discTypes);
    setPlot();
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

void PlotModel::processFrame(const FrameDTO& frameDTO)
{
    if (frameDTO.elapsedSimulationTimeUs == 0)
    {
        emitInitialHistogram(frameDTO);
        return;
    }
    else if (dataPointForPlotting_.elapsedTime_ == 0)
        dataPointForPlotting_.clear(); // Clear the datapoint used for initial histogram display

    DataPoint dataPoint = dataPointFromFrameDTO(frameDTO);
    storeDataPoint(dataPoint);
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
    DataPoint dataPointToAverage(simulation_->getSimulationConfig());
    int averagingCount = 0;

    for (const auto& dataPoint : dataPoints_)
    {
        // TODO This adds everything, but we only need the current plot category
        dataPointToAverage += dataPoint;
        ++averagingCount;

        if (dataPointToAverage.elapsedTime_ < plotTimeInterval_)
            continue;

        averageDataPoint(dataPointToAverage, averagingCount);
        const auto& activeMap = getActiveMap(dataPointToAverage);

        if (plotSum_)
        {
            auto sum = std::accumulate(activeMap.begin(), activeMap.end(), 0.0,
                                       [](double partialSum, const auto& pair) { return pair.second + partialSum; });
            fullPlotData.push_back({{"Sum", sum}});
        }
        else
            fullPlotData.push_back(getActiveMap(dataPointToAverage));

        dataPointToAverage.clear();
        averagingCount = 0;
    }

    emit setPlot(PlotWidget::LinePlotParams{.labels = labels_,
                                            .colors = colors_,
                                            .dataPoints = fullPlotData,
                                            .plotCategory = plotCategory_,
                                            .xStep = plotTimeInterval_});
}

void PlotModel::setHistogramPlot()
{
    Histogram histogram;
    const int requiredDataPoints = std::max(1, static_cast<int>(std::ceil(plotTimeInterval_ / storageTime_)));

    if (static_cast<int>(dataPoints_.size()) < requiredDataPoints)
        histogram = getVelocityHistogramFromDataPoint(dataPointForPlotting_, CalculateSum{.value = plotSum_});
    else
    {
        histogram = getVelocityHistogramFromDataPoint(dataPoints_.back(), CalculateSum{.value = plotSum_});

        for (int i = 1; i < requiredDataPoints; ++i)
            histogram += getVelocityHistogramFromDataPoint(dataPoints_[i], CalculateSum{.value = plotSum_});

        histogram /= requiredDataPoints;
    }

    emit setPlot(PlotWidget::HistogramParams{.labels = labels_, .colors = colors_, .histogram = histogram});
}

void PlotModel::setColorMapPlot()
{
    std::vector<Histogram> histograms;
    histograms.reserve(dataPoints_.size());

    DataPoint dataPointToAverage(simulation_->getSimulationConfig());
    int averagingCount = 0;

    for (const auto& dataPoint : dataPoints_)
    {
        dataPointToAverage += dataPoint;
        ++averagingCount;

        if (dataPointToAverage.elapsedTime_ < plotTimeInterval_)
            continue;

        averageDataPoint(dataPointToAverage, averagingCount);
        histograms.push_back(getVelocityHistogramFromDataPoint(dataPointToAverage, CalculateSum{.value = true}));

        dataPointToAverage.clear();
        averagingCount = 0;
    }

    // Simulation hasn't run yet, display plot for initial data
    if (histograms.empty())
        histograms.push_back(getVelocityHistogramFromDataPoint(dataPointForPlotting_, CalculateSum{.value = true}));

    emit setPlot(PlotWidget::ColorMapParams{.histograms = histograms, .xStep = plotTimeInterval_});
}

void PlotModel::updatePlot()
{
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
}

void PlotModel::updateLinePlot()
{
    const auto& activeMap = getActiveMap(dataPointForPlotting_);

    if (plotSum_)
    {
        auto sum = std::accumulate(activeMap.begin(), activeMap.end(), 0.0,
                                   [&](double partialSum, const auto& pair) { return pair.second + partialSum; });
        emit updatePlot(PlotWidget::LinePlotData{.dataPoint = {{"Sum", sum}}, .doReplot = true});
    }
    else
        emit updatePlot(PlotWidget::LinePlotData{.dataPoint = activeMap, .doReplot = true});
}

void PlotModel::updateHistogramPlot()
{
    auto histogram = getVelocityHistogramFromDataPoint(dataPointForPlotting_, CalculateSum{.value = plotSum_});
    emit updatePlot(PlotWidget::HistogramData{.histogram = histogram});
}

void PlotModel::updateColorMapPlot()
{
    auto histogram = getVelocityHistogramFromDataPoint(dataPointForPlotting_, CalculateSum{.value = true});
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

DataPoint PlotModel::dataPointFromFrameDTO(const FrameDTO& frameDTO)
{
    const auto& discTypeRegistry = simulation_->getDiscTypeRegistry();
    DataPoint dataPoint(simulation_->getSimulationConfig());

    auto& discs = frameDTO.discs_;

    for (const auto& [discType, collisionCount] : frameDTO.collisionCounts_)
        dataPoint.collisionCounts_[discTypeRegistry.getByID(discType).getName()] = static_cast<double>(collisionCount);

    dataPoint.elapsedTime_ = static_cast<double>(frameDTO.elapsedSimulationTimeUs) / 1'000'000.0;
    std::unordered_map<std::string, cell::Vector2d> momentumMap;

    for (const auto& disc : discs)
    {
        std::string discTypeName = discTypeRegistry.getByID(disc.getTypeID()).getName();
        ++dataPoint.discTypeCountMap_[discTypeName];
        dataPoint.totalKineticEnergyMap_[discTypeName] +=
            disc.getKineticEnergy(discTypeRegistry.getByID(disc.getTypeID()).getMass());
        momentumMap[discTypeName] += disc.getMomentum(discTypeRegistry.getByID(disc.getTypeID()).getMass());
        dataPoint.vxHistogram_(discTypeName, disc.getVelocity().x);
    }

    for (const auto& [discTypeName, momentum] : momentumMap)
        dataPoint.totalMomentumMap_[discTypeName] = cell::mathutils::abs(momentum);

    return dataPoint;
}

std::unordered_map<std::string, double> PlotModel::getActiveMap(const DataPoint& dataPoint)
{
    std::unordered_map<std::string, double> activeMap;

    switch (plotCategory_)
    {
    case PlotCategory::TypeCounts: activeMap = dataPoint.discTypeCountMap_; break;
    case PlotCategory::CollisionCounts: activeMap = dataPoint.collisionCounts_; break;
    case PlotCategory::AbsoluteMomentum: activeMap = dataPoint.totalMomentumMap_; break;
    case PlotCategory::KineticEnergy: activeMap = dataPoint.totalKineticEnergyMap_; break;
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
        dataPoints_.push_back(dataPointForStorage_);
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

Histogram PlotModel::sumHistogramStacks(const Histogram& histogram)
{
    const auto& categoryAxis = histogram.axis<0>();
    const auto& regularAxis = histogram.axis<1>();
    Histogram sumHistogram = makeHistogramWithCategories(histogram, {"Sum"});

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
    std::vector<std::string> activeDiscTypes;
    for (int i = 0; i < categoryAxis.size(); ++i)
    {
        const auto& discType = categoryAxis.value(i);
        if (activePlotDiscTypes_[discType])
            activeDiscTypes.push_back(discType);
    }

    Histogram filteredHistogram = makeHistogramWithCategories(histogram, activeDiscTypes);

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
    auto h = discardInactiveDiscTypes(dataPoint.vxHistogram_);
    if (calculateSum.value)
        h = sumHistogramStacks(h);

    return h;
}

Histogram PlotModel::makeHistogramWithCategories(const Histogram& source, const std::vector<std::string>& categories)
{
    const auto& regularAxis = source.axis<1>();

    return bh::make_histogram(bh::axis::category<std::string>(categories, "Disc types"),
                              bh::axis::regular<>(regularAxis.size(), regularAxis.value(0),
                                                  regularAxis.value(regularAxis.size()), regularAxis.metadata()));
}

void PlotModel::emitInitialHistogram(const FrameDTO& frameDTO)
{
    const bool simulationDataCollected =
        !dataPoints_.empty() || dataPointForStorage_.elapsedTime_ > 0 || dataPointForPlotting_.elapsedTime_ > 0;
    if (simulationDataCollected)
        return;

    // Store in case user switches to this plot later
    dataPointForPlotting_ = dataPointFromFrameDTO(frameDTO);
    if (plotCategory_ == PlotCategory::VelocityDistribution || plotCategory_ == PlotCategory::VelocityColorMap)
        setPlot();
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
