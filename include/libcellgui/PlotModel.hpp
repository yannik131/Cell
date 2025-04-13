#ifndef PLOTMODEL_HPP
#define PLOTMODEL_HPP

#include "FrameDTO.hpp"
#include "GlobalGUISettings.hpp"
#include "PlotCategories.hpp"

#include <QObject>

#include <SFML/System/Time.hpp>

/**
 * @brief Struct containing information from a FrameDTO relevant for the plot
 */
struct DataPoint
{
    long long elapsedTimeUs_ = 0;
    int collisionCount_ = 0;
    QMap<DiscType, double> totalMomentumMap_;
    QMap<DiscType, double> totalKineticEnergyMap_;
    QMap<DiscType, int> discTypeCountMap_;
};

class PlotModel : public QObject
{
    Q_OBJECT
public:
    PlotModel(QObject* parent = nullptr);

    void clear();

public slots:
    void receiveFrameDTO(const FrameDTO& frameDTO);

signals:
    void dataPointAdded(const DataPoint& dataPoint);
    void newPlotCreated(const QVector<DataPoint>& dataPoints);

private:
    /**
     * @brief Emits the newest data point to be added to the plot
     */
    void emitDataPoint(DataPoint& averagedDataPoint);

    /**
     * @brief Emits data for the full plot with all data points
     */
    void emitPlot();

private:
    /**
     * @brief Recently received DataPoints where sum of DataPoint::elapsedTimeUs_ < plotTimeInterval_
     */
    QVector<DataPoint> dataPointsToAverage_;

    /**
     * @brief All data points received from the simulation
     */
    QVector<DataPoint> dataPoints_;

    /**
     * @brief Accumulated time of the received FrameDTOs since the last data point was emitted
     */
    sf::Time elapsedWorldTimeSinceLastPlot_ = sf::Time::Zero;

    /**
     * @brief Alias for frequently accessed setting
     */
    const sf::Time& PlotTimeInterval = GlobalGUISettings::getGUISettings().plotTimeInterval_;
};

#endif /* PLOTMODEL_HPP */
