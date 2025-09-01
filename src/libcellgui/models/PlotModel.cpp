#include "PlotModel.hpp"
#include "MathUtils.hpp"

namespace
{

const cell::DiscTypeMap<double>& getActiveMap(const DataPoint& dataPoint)
{
}

void averageDataPoint(DataPoint& dataPoint, int length)
{
    double dt = sf::microseconds(dataPoint.elapsedTimeUs_).asSeconds();

    // Collisions per second = All registered collisions / dt
    dataPoint.collisionCounts_ /= dt;

    dataPoint.totalKineticEnergyMap_ /= dt;
    dataPoint.totalMomentumMap_ /= dt;

    // This is time-independent, divide by just number of data points to get average number of disc types for each data
    // point
    dataPoint.discTypeCountMap_ /= length;
}

} // namespace

PlotModel::PlotModel(QObject* parent)
    : QObject(parent)
{
    // With a simulation time step of 1ms, we get 1000 data points each second
    // We'll reserve enough space for 5 minutes of plotting, 5*60*1000
    dataPoints_.reserve(300000);
}

void PlotModel::clear()
{
    dataPoints_.clear();
    dataPointBeingAveraged_ = DataPoint();
    averagingCount_ = 0;

    emitPlot();
}

void PlotModel::addDataPointFromFrameDTO(const FrameDTO& frameDTO)
{
    // Elapsed time 0 means this DTO was only emitted for a redraw
    if (frameDTO.elapsedSimulationTimeUs == 0)
        return;

    DataPoint dataPoint = dataPointFromFrameDTO(frameDTO);

    dataPointBeingAveraged_ += dataPoint;
    ++averagingCount_;

    dataPoints_.push_back(std::move(dataPoint));

    plotAveragedDataPoint();
}

void PlotModel::emitDataPoint(const DataPoint& averagedDataPoint)
{
    // https://stackoverflow.com/questions/8455887/stack-object-qt-signal-and-parameter-as-reference
    emit dataPointAdded(getActiveMap(averagedDataPoint));
}

void PlotModel::emitPlot()
{
    QVector<cell::DiscTypeMap<double>> fullPlotData;
    DataPoint dataPointToAverage;
    int averagingCount = 0;

    for (const auto& dataPoint : dataPoints_)
    {
        dataPointToAverage += dataPoint;
        ++averagingCount;

        if (dataPointToAverage.elapsedTimeUs_ >= PlotTimeInterval.asMicroseconds())
        {
            averageDataPoint(dataPointToAverage, averagingCount);
            fullPlotData.append(getActiveMap(dataPointToAverage));

            dataPointToAverage = DataPoint();
            averagingCount = 0;
        }
    }

    emit newPlotCreated(fullPlotData);
}

DataPoint PlotModel::dataPointFromFrameDTO(const FrameDTO& frameDTO)
{
    DataPoint dataPoint;

    for (const auto& [discType, collisionCount] : frameDTO.collisionCounts_)
        dataPoint.collisionCounts_[discType] = static_cast<double>(collisionCount);
    dataPoint.elapsedTimeUs_ = frameDTO.elapsedSimulationTimeUs;

    cell::DiscTypeMap<sf::Vector2d> totalMomentums;

    for (const auto& disc : frameDTO.discs_)
    {
        ++dataPoint.discTypeCountMap_[disc.getDiscTypeID()];
        dataPoint.totalKineticEnergyMap_[disc.getDiscTypeID()] += disc.getKineticEnergy();
        // totalMomentums[disc.getDiscTypeID()] += disc.getMomentum();
    }

    for (const auto& [discType, totalMomentum] : totalMomentums)
        dataPoint.totalMomentumMap_[discType] = std::hypot(totalMomentum.x, totalMomentum.y);

    return dataPoint;
}

void PlotModel::plotAveragedDataPoint()
{
    if (dataPointBeingAveraged_.elapsedTimeUs_ < PlotTimeInterval.asMicroseconds())
        return;

    averageDataPoint(dataPointBeingAveraged_, averagingCount_);

    emitDataPoint(dataPointBeingAveraged_);

    dataPointBeingAveraged_ = DataPoint();
    averagingCount_ = 0;
}

DataPoint& operator+=(DataPoint& lhs, const DataPoint& rhs)
{
    auto dt = static_cast<double>(rhs.elapsedTimeUs_) / 1'000'000.0;

    lhs.elapsedTimeUs_ += rhs.elapsedTimeUs_;
    lhs.collisionCounts_ += rhs.collisionCounts_;
    lhs.discTypeCountMap_ += rhs.discTypeCountMap_;

    // To get values per second, we multiply by elapsed time in seconds
    lhs.totalKineticEnergyMap_ += (rhs.totalKineticEnergyMap_ * dt);
    lhs.totalMomentumMap_ += (rhs.totalMomentumMap_ * dt);

    return lhs;
}
