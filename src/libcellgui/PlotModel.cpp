#include "PlotModel.hpp"
#include "GlobalGUISettings.hpp"

template <typename T> QMap<DiscType, T> operator+=(QMap<DiscType, T>& a, const QMap<DiscType, T>& b)
{
    for (auto iter = a.begin(); iter != a.end(); ++iter)
        iter.value() += b[iter.key()];
}

template <typename T, typename DivisorType> QMap<DiscType, T> operator/=(QMap<DiscType, T>& a, const DivisorType& b)
{
    for (auto iter = a.begin(); iter != a.end(); ++iter)
        iter.value() /= b;
}

/**
 * @brief Calculates the average of a list of data points
 */
DataPoint averageDataPoints(const QVector<DataPoint>& dataPoints)
{
    DataPoint average;
    for (const auto& dataPoint : dataPoints)
    {
        average.collisionCount_ += dataPoint.collisionCount_;
        average.totalKineticEnergyMap_ += dataPoint.totalKineticEnergyMap_;
        average.totalMomentumMap_ += dataPoint.totalMomentumMap_;
        average.discTypeCountMap_ += dataPoint.discTypeCountMap_;
    }

    average.collisionCount_ /= dataPoints.size();
    average.totalKineticEnergyMap_ /= dataPoints.size();
    average.totalMomentumMap_ /= dataPoints.size();
    average.discTypeCountMap_ /= dataPoints.size();

    return average;
}

/**
 * @brief Removes inactive disc types from all maps in the given data point
 */
void removeInactiveDiscTypes(DataPoint& dataPoint)
{
    const auto& activeDiscTypesMap = GlobalGUISettings::getGUISettings().discTypesPlotMap_;

    for (auto iter = activeDiscTypesMap.begin(); iter != activeDiscTypesMap.end(); ++iter)
    {
        if (!iter.value())
        {
            dataPoint.totalKineticEnergyMap_.remove(iter.key());
            dataPoint.totalMomentumMap_.remove(iter.key());
            dataPoint.discTypeCountMap_.remove(iter.key());
        }
    }
}

PlotModel::PlotModel(QObject* parent)
    : QObject(parent)
{
    // With a simulation time step of 5ms, we get 200 data points each second
    // We'll reserve enough space for 5 minutes of plotting, 5*60*200
    dataPoints_.reserve(60000);

    connect(&GlobalGUISettings::get(), &GlobalGUISettings::plotResetRequired, this, &PlotModel::emitPlot);
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
    dataPoint.collisionCount_ = frameDTO.collisionCount_;
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
        const auto& averagedDataPoint = averageDataPoints(dataPointsToAverage_);
        dataPoints_.append(std::move(dataPointsToAverage_));
        dataPointsToAverage_.clear();
        elapsedWorldTimeSinceLastPlot_ -= PlotTimeInterval;

        emitDataPoint(averagedDataPoint);
    }
}

void PlotModel::emitDataPoint(DataPoint& averagedDataPoint)
{
    removeInactiveDiscTypes(averagedDataPoint);

    emit dataPointAdded(averagedDataPoint);
}

void PlotModel::emitPlot()
{
    QVector<DataPoint> fullPlotData;
    QVector<DataPoint> dataPointsToAverage;
    sf::Time elapsedTime = sf::Time::Zero;

    for (const auto& dataPoint : dataPoints_)
    {
        dataPointsToAverage.push_back(dataPoint);

        elapsedTime += sf::microseconds(dataPoint.elapsedTimeUs_);
        if (elapsedTime >= PlotTimeInterval)
        {
            DataPoint averagedDataPoint = averageDataPoints(dataPointsToAverage_);
            removeInactiveDiscTypes(averagedDataPoint);
            fullPlotData.append(averagedDataPoint);
            dataPointsToAverage_.clear();
            elapsedTime -= PlotTimeInterval;
        }
    }

    emit newPlotCreated(fullPlotData);
}