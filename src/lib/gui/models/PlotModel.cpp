#include "models/PlotModel.hpp"
#include "PlotModel.hpp"
#include "cell/MathUtils.hpp"
#include "core/AbstractSimulationBuilder.hpp"

/**
 * - Disc types changed:
 *  - Identify remaining active disc types for plot
 *  - If there are none, plot is empty (no active disc types for plotting)
 *  - If there are, replot
 * - Any other changes that triggered a simulation reset (context rebuild):
 *  - Empty the plot, keep everything else
 * - Plot category, plotting sum or plot time interval changed: Re-create current plot
 */

namespace
{

void averageDataPoint(DataPoint& dataPoint, int length)
{
    double dt = dataPoint.elapsedTime_;

    // Collisions per second = All registered collisions / dt
    dataPoint.collisionCounts_ /= dt;

    dataPoint.totalKineticEnergyMap_ /= dt;
    dataPoint.totalMomentumMap_ /= dt;

    // This is time-independent, divide by just number of data points to get average number of disc types for each data
    // point
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
            labels_.clear();
            colors_.clear();
            for (const auto& discType : config.discTypes)
            {
                labels_.push_back(discType.name);
                colors_.push_back(colorMap.at(discType.name));
            }
            emit createGraphs(labels_, colors_);
        });
}

void PlotModel::setPlotCategory(PlotCategory plotCategory)
{
    plotCategory_ = plotCategory;
    emitPlot();
}

void PlotModel::setPlotTimeInterval(int valueMilliseconds)
{
    plotTimeInterval_ = static_cast<double>(valueMilliseconds) / 1000.0;
    emitPlot();
}

void PlotModel::setPlotSum(bool value)
{
    plotSum_ = value;
    emitPlot();
}

void PlotModel::reset()
{
    dataPoints_.clear();
    dataPointBeingAveraged_ = DataPoint();
    averagingCount_ = 0;
}

void PlotModel::processFrame(const FrameDTO& frameDTO)
{
    // Elapsed time 0 means this DTO was only emitted for a redraw
    if (frameDTO.elapsedSimulationTimeUs == 0)
        return;

    DataPoint dataPoint = dataPointFromFrameDTO(frameDTO);
    dataPointBeingAveraged_ += dataPoint;
    ++averagingCount_;

    if (dataPointBeingAveraged_.elapsedTime_ >= plotTimeInterval_)
    {
        averageDataPoint(dataPointBeingAveraged_, averagingCount_);
        dataPoints_.push_back(dataPointBeingAveraged_);
        double x = static_cast<double>(dataPoints_.size()) * plotTimeInterval_;

        emit addDataPoint(getActiveMap(dataPointBeingAveraged_), x, DoReplot{true});

        dataPointBeingAveraged_ = {};
        averagingCount_ = 0;
    }
}

void PlotModel::emitPlot()
{
    std::vector<std::unordered_map<std::string, double>> fullPlotData;
    DataPoint dataPointToAverage;
    int averagingCount = 0;

    for (const auto& dataPoint : dataPoints_)
    {
        dataPointToAverage += dataPoint;
        ++averagingCount;

        if (dataPointToAverage.elapsedTime_ >= plotTimeInterval_)
        {
            averageDataPoint(dataPointToAverage, averagingCount);
            fullPlotData.push_back(getActiveMap(dataPointToAverage));
            dataPointToAverage = {};
            averagingCount = 0;
        }
    }

    emit replaceDataPoints(fullPlotData, plotTimeInterval_);
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
    switch (plotCategory_)
    {
    case PlotCategory::TypeCounts:
        return dataPoint.discTypeCountMap_;
    case PlotCategory::CollisionCounts:
        return dataPoint.collisionCounts_;
    case PlotCategory::AbsoluteMomentum:
        return dataPoint.totalMomentumMap_;
    case PlotCategory::KineticEnergy:
        return dataPoint.totalKineticEnergyMap_;
    default:
        return {};
    }
}

DataPoint& operator+=(DataPoint& lhs, const DataPoint& rhs)
{
    lhs.elapsedTime_ += rhs.elapsedTime_;
    lhs.collisionCounts_ += rhs.collisionCounts_;
    lhs.discTypeCountMap_ += rhs.discTypeCountMap_;

    // To get values per second, we multiply by elapsed time in seconds
    lhs.totalKineticEnergyMap_ += (rhs.totalKineticEnergyMap_ * rhs.elapsedTime_);
    lhs.totalMomentumMap_ += (rhs.totalMomentumMap_ * rhs.elapsedTime_);

    return lhs;
}
