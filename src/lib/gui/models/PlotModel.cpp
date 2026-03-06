#include "models/PlotModel.hpp"
#include "cell/MathUtils.hpp"
#include "core/Simulation.hpp"
#include "core/Utility.hpp"

#include <cmath>
#include <unordered_set>

namespace
{

void averageDataPoint(DataPoint& dataPoint, int length)
{
    double dt = dataPoint.elapsedTime_;

    // Collisions per second = All registered collisions / dt
    utility::divideValuesBy(dataPoint.collisionCounts_, dt);

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
        {PlotCategory::VelocityDistribution, 2}, {PlotCategory::VelocityHeatMap, 3}};

    const auto oldPlotCategory = plotCategory_;
    plotCategory_ = plotCategory;

    if (graphType.at(oldPlotCategory) != graphType.at(plotCategory))
        emitGraphs();

    emitWholePlot();
}

void PlotModel::setPlotTimeInterval(int valueMilliseconds)
{
    // To reduce the number of datapoints, we store 100ms intervals
    if (valueMilliseconds < 100)
        throw ExceptionWithLocation("The plot time interval must be at least 100ms");

    if (valueMilliseconds % 100 != 0)
        throw ExceptionWithLocation("The plot time interval must be a multiple of 100ms");

    plotTimeInterval_ = static_cast<double>(valueMilliseconds) / 1000.0;
    emitWholePlot();
}

void PlotModel::setPlotSum(bool value)
{
    plotSum_ = value;
    emitGraphs();
    emitWholePlot();
}

void PlotModel::reset()
{
    dataPoints_.clear();
    dataPointForStorage_.clear();
    dataPointForPlotting_.clear();
    averagingCount_ = 0;

    updateActivePlotDiscTypes(simulation_->getSimulationConfig().discTypes);

    emitGraphs();
    emitWholePlot();
}

void PlotModel::setActivePlotDiscTypes(const std::vector<std::string>& activeDiscTypeNames)
{
    activePlotDiscTypes_.clear();
    for (const auto& name : activeDiscTypeNames)
        activePlotDiscTypes_[name] = true;

    emitGraphs();
    emitWholePlot();
}

const std::map<std::string, bool>& PlotModel::getActivePlotDiscTypesMap() const
{
    return activePlotDiscTypes_;
}

void PlotModel::processFrame(const FrameDTO& frameDTO)
{
    // Elapsed time 0 means this DTO was only emitted for a redraw
    if (frameDTO.elapsedSimulationTimeUs == 0)
        return;

    DataPoint dataPoint = dataPointFromFrameDTO(frameDTO);

    storeDataPoint(dataPoint);

    if (dataPointForPlotting_.elapsedTime_ >= plotTimeInterval_)
    {
        emitPlotPart();
        dataPointForPlotting_.clear();
    }
}

void PlotModel::emitWholePlot()
{
    switch (plotCategory_)
    {
    case PlotCategory::TypeCounts:
    case PlotCategory::AbsoluteMomentum:
    case PlotCategory::CollisionCounts:
    case PlotCategory::KineticEnergy: emitLinePlot(); break;
    case PlotCategory::VelocityDistribution: emitHistogram(); break;
    case PlotCategory::VelocityHeatMap: emitHeatMap(); break;
    default: throw ExceptionWithLocation("Invalid plot category");
    }
}

void PlotModel::emitLinePlot()
{
    std::vector<std::unordered_map<std::string, double>> fullPlotData;
    DataPoint dataPointToAverage(simulation_->getSimulationConfig());
    int averagingCount = 0;
    double timeStep = simulation_->getSimulationConfig().simulationTimeStep;
    const int dataPointsPerStoredPoint = static_cast<int>(std::ceil(storageTime_ / timeStep));

    for (const auto& dataPoint : dataPoints_)
    {
        // TODO This adds everything, but we only need the current plot category
        dataPointToAverage += dataPoint;
        ++averagingCount;

        if (dataPointToAverage.elapsedTime_ < plotTimeInterval_)
            continue;

        averageDataPoint(dataPointToAverage, averagingCount * dataPointsPerStoredPoint);
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

    emit createLinePlots(labels_, colors_);
    emit linePlotPoints(fullPlotData, plotTimeInterval_);
}

void PlotModel::emitHistogram()
{
    if (plotSum_)
    {
        const auto h = sumHistogramStacks(dataPointForPlotting_.vxHistogram_);
        emit histogram(h);
        return;
    }

    emit histogram(dataPointForPlotting_.vxHistogram_);
}

void PlotModel::emitHeatMap()
{
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

    if (dataPointForStorage_.elapsedTime_ >= storageTime_)
    {
        dataPoints_.push_back(dataPointForStorage_);
        dataPointForStorage_.clear();
    }

    dataPointForPlotting_ += dataPoint;
    ++averagingCount_;

    if (dataPointForPlotting_.elapsedTime_ >= plotTimeInterval_)
    {
        averageDataPoint(dataPointForPlotting_, averagingCount_);
        averagingCount_ = 0;
    }
}

void PlotModel::emitPlotPart()
{
    switch (plotCategory_)
    {
    case PlotCategory::TypeCounts:
    case PlotCategory::AbsoluteMomentum:
    case PlotCategory::CollisionCounts:
    case PlotCategory::KineticEnergy: emitLinePlotPoints(); break;
    case PlotCategory::VelocityDistribution: emitHistogram(); break;
    case PlotCategory::VelocityHeatMap: emitHeatMapColumn(); break;
    default: throw ExceptionWithLocation("Invalid plot category");
    }
}

void PlotModel::emitLinePlotPoints()
{
    const auto& activeMap = getActiveMap(dataPointForPlotting_);

    if (plotSum_)
    {
        auto sum = std::accumulate(activeMap.begin(), activeMap.end(), 0.0,
                                   [&](double partialSum, const auto& pair) { return pair.second + partialSum; });
        emit linePlotPoint({{"Sum", sum}}, plotTimeInterval_, DoReplot{true});
    }
    else
        emit linePlotPoint(activeMap, plotTimeInterval_, DoReplot{true});
}

void PlotModel::emitHeatMapColumn()
{
}

void PlotModel::emitGraphs()
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

    switch (plotCategory_)
    {
    case PlotCategory::TypeCounts:
    case PlotCategory::AbsoluteMomentum:
    case PlotCategory::CollisionCounts:
    case PlotCategory::KineticEnergy: emit createLinePlots(labels_, colors_); break;
    case PlotCategory::VelocityDistribution:
        emit createHistogram(labels_, colors_, dataPointForPlotting_.vxHistogram_);
        break;
    case PlotCategory::VelocityHeatMap:
    default: throw ExceptionWithLocation("Invalid plot category");
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
    Histogram sumHistogram =
        bh::make_histogram(bh::axis::category<std::string>(std::vector<std::string>{{"Sum"}}, "Disc types"),
                           bh::axis::regular<>(regularAxis.size(), regularAxis.value(0),
                                               regularAxis.value(regularAxis.size()), regularAxis.metadata()));

    for (int i = 0; i < regularAxis.size(); ++i)
    {
        double sum = 0.0;
        for (int j = 0; j < categoryAxis.size(); ++j)
            sum += histogram.at(j, i);

        sumHistogram.at(0, i) = sum;
    }

    return sumHistogram;
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
