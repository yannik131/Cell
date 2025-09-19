#include "models/PlotModel.hpp"
#include "PlotModel.hpp"
#include "cell/MathUtils.hpp"
#include "core/AbstractSimulationBuilder.hpp"

#include <cmath>
#include <unordered_set>

namespace
{

void averageDataPoint(DataPoint& dataPoint, int length)
{
    double dt = dataPoint.elapsedTime_;

    // Collisions per second = All registered collisions / dt
    dataPoint.collisionCounts_ /= dt;

    dataPoint.totalKineticEnergyMap_ /= length;
    dataPoint.totalMomentumMap_ /= length;
    dataPoint.discTypeCountMap_ /= length;
}

} // namespace

PlotModel::PlotModel(QObject* parent, AbstractSimulationBuilder* abstractSimulationBuilder)
    : QObject(parent)
    , abstractSimulationBuilder_(abstractSimulationBuilder)
{
    // With a simulation time step of 1ms, we get 1000 data points each second
    // With an averaging time of 100ms, we save 10 datapoints for 1 second
    // We'll reserve enough space for 5 minutes of plotting, 5*60*10
    dataPoints_.reserve(3000);

    abstractSimulationBuilder_->registerConfigObserver(
        [&](const cell::SimulationConfig& config, const std::map<std::string, sf::Color>& colorMap)
        {
            reset();
            updateActivePlotDiscTypes(config.discTypes);
            emitGraphs();
        });
}

void PlotModel::setPlotCategory(PlotCategory plotCategory)
{
    plotCategory_ = plotCategory;
    emitPlot();
}

void PlotModel::setPlotTimeInterval(int valueMilliseconds)
{
    // To reduce the number of datapoints, we store 100ms intervals
    if (valueMilliseconds < 100)
        throw std::invalid_argument("The plot time interval must be at least 100ms");

    if (valueMilliseconds % 100 != 0)
        throw std::invalid_argument("The plot time interval must be a multiple of 100ms");

    plotTimeInterval_ = static_cast<double>(valueMilliseconds) / 1000.0;
    emitPlot();
}

void PlotModel::setPlotSum(bool value)
{
    plotSum_ = value;
    emitGraphs();
    emitPlot();
}

void PlotModel::reset()
{
    dataPoints_.clear();
    dataPointForStorage_ = {};
    dataPointForPlotting_ = {};
    averagingCount_ = 0;
}

void PlotModel::setActivePlotDiscTypes(const std::vector<std::string>& activeDiscTypeNames)
{
    activePlotDiscTypes_.clear();
    for (const auto& name : activeDiscTypeNames)
        activePlotDiscTypes_[name] = true;

    emitGraphs();
    emitPlot();
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
    plotDataPoint(dataPoint);
}

void PlotModel::emitPlot()
{
    std::vector<std::unordered_map<std::string, double>> fullPlotData;
    DataPoint dataPointToAverage;
    int averagingCount = 0;
    double timeStep = abstractSimulationBuilder_->getSimulationConfig().setup.simulationTimeStep;
    const int dataPointsPerStoredPoint = static_cast<int>(std::ceil(storageTime_ / timeStep));

    for (const auto& dataPoint : dataPoints_)
    {
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

        dataPointToAverage = {};
        averagingCount = 0;
    }

    emit createGraphs(labels_, colors_);
    emit addDataPoints(fullPlotData, plotTimeInterval_);
}

DataPoint PlotModel::dataPointFromFrameDTO(const FrameDTO& frameDTO)
{
    DataPoint dataPoint;
    auto discTypeResolver = abstractSimulationBuilder_->getDiscTypeResolver();

    for (const auto& [discType, collisionCount] : frameDTO.collisionCounts_)
        dataPoint.collisionCounts_[discTypeResolver(discType).getName()] = static_cast<double>(collisionCount);

    dataPoint.elapsedTime_ = static_cast<double>(frameDTO.elapsedSimulationTimeUs) / 1'000'000;

    for (const auto& disc : frameDTO.discs_)
    {
        std::string discTypeName = discTypeResolver(disc.getDiscTypeID()).getName();
        ++dataPoint.discTypeCountMap_[discTypeName];
        dataPoint.totalKineticEnergyMap_[discTypeName] += disc.getKineticEnergy(discTypeResolver);
        dataPoint.totalMomentumMap_[discTypeName] += disc.getAbsoluteMomentum(discTypeResolver);
    }

    return dataPoint;
}

std::unordered_map<std::string, double> PlotModel::getActiveMap(const DataPoint& dataPoint)
{
    std::unordered_map<std::string, double> activeMap;

    switch (plotCategory_)
    {
    case PlotCategory::TypeCounts:
        activeMap = dataPoint.discTypeCountMap_;
        break;
    case PlotCategory::CollisionCounts:
        activeMap = dataPoint.collisionCounts_;
        break;
    case PlotCategory::AbsoluteMomentum:
        activeMap = dataPoint.totalMomentumMap_;
        break;
    case PlotCategory::KineticEnergy:
        activeMap = dataPoint.totalKineticEnergyMap_;
        break;
    default:
        activeMap = {};
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
        dataPointForStorage_ = {};
    }
}

void PlotModel::plotDataPoint(const DataPoint& dataPoint)
{
    dataPointForPlotting_ += dataPoint;
    ++averagingCount_;

    if (dataPointForPlotting_.elapsedTime_ < plotTimeInterval_)
        return;

    averageDataPoint(dataPointForPlotting_, averagingCount_);
    const auto& activeMap = getActiveMap(dataPointForPlotting_);

    if (plotSum_)
    {
        auto sum = std::accumulate(activeMap.begin(), activeMap.end(), 0.0,
                                   [&](double partialSum, const auto& pair) { return pair.second + partialSum; });
        emit addDataPoint({{"Sum", sum}}, plotTimeInterval_, DoReplot{true});
    }
    else
        emit addDataPoint(activeMap, plotTimeInterval_, DoReplot{true});

    dataPointForPlotting_ = {};
    averagingCount_ = 0;
}

void PlotModel::emitGraphs()
{
    labels_.clear();
    colors_.clear();

    const auto& config = abstractSimulationBuilder_->getSimulationConfig();
    const auto& colorMap = abstractSimulationBuilder_->getDiscTypeColorMap();

    if (plotSum_)
    {
        labels_.push_back("Sum");
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

    emit createGraphs(labels_, colors_);
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

DataPoint& operator+=(DataPoint& lhs, const DataPoint& rhs)
{
    lhs.elapsedTime_ += rhs.elapsedTime_;
    lhs.collisionCounts_ += rhs.collisionCounts_;
    lhs.discTypeCountMap_ += rhs.discTypeCountMap_;

    lhs.totalKineticEnergyMap_ += rhs.totalKineticEnergyMap_;
    lhs.totalMomentumMap_ += rhs.totalMomentumMap_;

    return lhs;
}
