#include "PlotModel.hpp"
#include "GlobalSettings.hpp"

#include <QDebug>

/**
 * @brief Calculates the average of a list of data points
 */
DataPoint averageDataPoints(const QVector<DataPoint>& dataPoints)
{
    DataPoint average;
    for (const auto& dataPoint : dataPoints)
    {
        average.collisionCount_ += dataPoint.collisionCount_;
        average.totalKineticEnergy_ += dataPoint.totalKineticEnergy_;
        average.totalMomentum_ += dataPoint.totalMomentum_;
        average.discTypeCount_ += dataPoint.discTypeCount_;
    }

    average.collisionCount_ /= dataPoints.size();
    average.totalKineticEnergy_ /= dataPoints.size();
    average.totalMomentum_ /= dataPoints.size();
    average.discTypeCount_ /= dataPoint.size();

    return average;
}

/**
 * @brief Removes inactive disc types from all maps in the given data point
 */
void removeInactiveDiscTypes(DataPoint& dataPoint, const QMap<DiscType, bool> activeDiscTypesMap)
{
    for (auto iter = activeDiscTypesMap.begin(); iter != activeDiscTypesMap.end(); ++iter)
    {
        if (!iter.value())
        {
            dataPoint.totalKineticEnergy_.remove(iter.key());
            dataPoint.totalMomentum_.remove(iter.key());
            dataPoint.discTypeCount_.remove(iter.key());
        }
    }
}

PlotModel::PlotModel(QObject* parent)
    : QObject(parent)
    , currentPlotCategory_(SupportedPlotCategories.first())
    , plotTimeInterval_(GlobalSettings::getSettings().plotTimeInterval_)
{
    // With a simulation time step of 5ms, we get 200 data points each second
    // We'll reserve enough space for 5 minutes of plotting, 5*60*200
    plotDataPoints_.reserve(60000);
    currentPlotCategory_ = SupportedPlotCategories.first();
    for (const auto& [discType, _] : GlobalSettings::getSettings().discTypeDistribution_)
        activeDiscTypes_.push_back(discType);
}

void PlotModel::setCurrentPlotCategory(const QString& plotCategoryName)
{
    const auto& plotCategory = NamePlotCategoryMapping[plotCategoryName];
    if (plotCategory == currentPlotCategory_)
        return;

    currentPlotCategory_ = plotCategory;
    emitPlot();
}

void PlotModel::receiveSelectedDiscTypeNames(const QStringList& selectedDiscTypeNames)
{
    activeDiscTypesMap_.clear();

    QVector<DiscType> activeDiscTypes;
    for (const auto& selectedDiscTypeName : selectedDiscTypeNames)
        activeDiscTypes.push_back(GlobalSettings::getDiscTypeByName(selectedDiscTypeName.toStdString()));

    for (const auto& [discType, _] : GlobalSettings::getSettings().discTypeDistribution_)
    {
        auto iter = std::find(activeDiscTypes.begin(), activeDiscTypes.end(), discType);
        activeDiscTypesMap_[discType] = iter != activeDiscTypes.end();
    }

    emitPlot();
}

void PlotModel::plotTimeIntervalChanged(const sf::Time& plotTimeInterval)
{
    plotTimeInterval_ = plotTimeInterval;
    emitPlot();
}

void PlotModel::receiveFrameDTO(const FrameDTO& frameDTO)
{
    DataPoint dataPoint;
    dataPoint.collisionCount_ = frameDTO.collisionCount_;
    dataPoint.elapsedTimeUs_ = frameDTO.simulationTimeStepUs;

    for (const auto& disc : frameDTO.discs_)
    {
        ++dataPoint.discTypeCount_[disc.getType()];
        dataPoint.totalKineticEnergy_[disc.getType()] += disc.getKineticEnergy();
        dataPoint.totalMomentum_[disc.getType()] += disc.getAbsoluteMomentum();
    }

    dataPointsToAverage_.push_back(dataPoint);

    elapsedWorldTimeSinceLastPlot_ += sf::microseconds(frameDTO.simulationTimeStepUs);
    if (elapsedWorldTimeSinceLastPlot_ >= plotTimeInterval_)
    {
        const auto& averagedDataPoint = averageDataPoints(dataPointsToAverage_);
        dataPoints_.append(std::move(dataPointsToAverage_));
        dataPointsToAverage_.clear();
        elapsedWorldTimeSinceLastPlot_ -= plotTimeInterval_;

        emitDataPoint(averagedDataPoint, currentPlotCategory_);
    }
}

void PlotModel::emitDataPoint(DataPoint averagedDataPoint)
{
    removeInactiveDiscTypes(averagedDataPoint);

    emit dataPointAdded(averagedDataPoint, currentPlotCategory_);
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
        if (elapsedTime >= plotTimeInterval_)
        {
            DataPoint averagedDataPoint = averageDataPoints(dataPointsToAverage_);
            removeInactiveDiscTypes(averagedDataPoint);
            fullPlotData.append(averagedDataPoint);
            dataPointsToAverage_.clear();
            elapsedTime -= plotTimeInterval_;
        }
    }

    emit newPlotCreated(fullPlotData, currentPlotCategory_);
}