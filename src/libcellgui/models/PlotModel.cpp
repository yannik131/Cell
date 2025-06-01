#include "PlotModel.hpp"
#include "GlobalGUISettings.hpp"
#include "GlobalSettingsFunctor.hpp"
#include "MathUtils.hpp"

const DiscType::map<double>& getActiveMap(const DataPoint& dataPoint)
{
    switch (GlobalGUISettings::getGUISettings().currentPlotCategory_)
    {
    case PlotCategory::TotalAbsoluteImpulse:
        return dataPoint.totalMomentumMap_;
    case PlotCategory::TotalCollisionCount:
        return dataPoint.collisionCounts_;
    case PlotCategory::TotalKineticEnergy:
        return dataPoint.totalKineticEnergyMap_;
    case PlotCategory::TypeCounts:
        return dataPoint.discTypeCountMap_;
    default:
        throw ExceptionWithLocation("Unknown plot category selected");
    }
}

/**
 * @brief Calculates the average of a list of data points
 * @todo maybe use a map<PlotCategory, map<DiscType, double>> to avoid duplication?
 */
DataPoint averageDataPoints(const QVector<DataPoint>& dataPoints)
{
    DataPoint average;
    for (const auto& dataPoint : dataPoints)
    {
        average.collisionCounts_ += dataPoint.collisionCounts_;
        average.totalKineticEnergyMap_ += dataPoint.totalKineticEnergyMap_;
        average.totalMomentumMap_ += dataPoint.totalMomentumMap_;
        average.discTypeCountMap_ += dataPoint.discTypeCountMap_;
    }

    average.collisionCounts_ /= dataPoints.size();
    average.totalKineticEnergyMap_ /= dataPoints.size();
    average.totalMomentumMap_ /= dataPoints.size();
    average.discTypeCountMap_ /= dataPoints.size();

    return average;
}

// TODO Bug: Run simulation, then set frequency of some discs to 0, reset, plot window is not resetted correctly
PlotModel::PlotModel(QObject* parent)
    : QObject(parent)
{
    // With a simulation time step of 5ms, we get 200 data points each second
    // We'll reserve enough space for 5 minutes of plotting, 5*60*200
    dataPoints_.reserve(60000);

    connect(&GlobalGUISettings::get(), &GlobalGUISettings::plotResetRequired, this, &PlotModel::emitPlot);
    connect(&GlobalSettingsFunctor::get(), &GlobalSettingsFunctor::numberOfDiscsChanged, this, &PlotModel::clear);
    // Updating the disc type distribution will cause the GlobalGUISettings to emit plotResetRequired, so no need to
    // connect to it here
}

void PlotModel::clear()
{
    dataPointsToAverage_.clear();
    dataPoints_.clear();
    elapsedWorldTimeSinceLastPlot_ = sf::Time::Zero;

    emitPlot();
}

void PlotModel::receiveFrameDTO(const FrameDTO& frameDTO)
{
    DataPoint dataPoint;

    for (const auto& [discType, collisionCount] : frameDTO.collisionCounts_)
        dataPoint.collisionCounts_[discType] = static_cast<double>(collisionCount);
    dataPoint.elapsedTimeUs_ = frameDTO.simulationTimeStepUs;

    for (const auto& disc : frameDTO.discs_)
    {
        ++dataPoint.discTypeCountMap_[disc.getType()];
        dataPoint.totalKineticEnergyMap_[disc.getType()] += disc.getKineticEnergy();
        dataPoint.totalMomentumMap_[disc.getType()] += disc.getAbsoluteMomentum();
    }

    dataPointsToAverage_.push_back(dataPoint);

    elapsedWorldTimeSinceLastPlot_ += sf::microseconds(frameDTO.simulationTimeStepUs);
    if (elapsedWorldTimeSinceLastPlot_ >= PlotTimeInterval)
    {
        auto averagedDataPoint = averageDataPoints(dataPointsToAverage_);
        dataPoints_.append(std::move(dataPointsToAverage_));
        dataPointsToAverage_.clear();
        elapsedWorldTimeSinceLastPlot_ -= PlotTimeInterval;

        emitDataPoint(averagedDataPoint);
    }
}

void PlotModel::emitDataPoint(DataPoint& averagedDataPoint)
{
    // https://stackoverflow.com/questions/8455887/stack-object-qt-signal-and-parameter-as-reference
    emit dataPointAdded(getActiveMap(averagedDataPoint));
}

void PlotModel::emitPlot()
{
    QVector<DiscType::map<double>> fullPlotData;
    QVector<DataPoint> dataPointsToAverage;
    sf::Time elapsedTime = sf::Time::Zero;

    for (const auto& dataPoint : dataPoints_)
    {
        dataPointsToAverage.push_back(dataPoint);

        elapsedTime += sf::microseconds(dataPoint.elapsedTimeUs_);
        if (elapsedTime >= PlotTimeInterval)
        {
            DataPoint averagedDataPoint = averageDataPoints(dataPointsToAverage);
            fullPlotData.append(getActiveMap(averagedDataPoint));
            dataPointsToAverage.clear();
            elapsedTime -= PlotTimeInterval;
        }
    }

    emit newPlotCreated(fullPlotData);
}