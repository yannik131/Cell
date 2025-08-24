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
    cell::DiscTypeMap<double> collisionCounts_;
    cell::DiscTypeMap<double> totalMomentumMap_;
    cell::DiscTypeMap<double> totalKineticEnergyMap_;
    cell::DiscTypeMap<double> discTypeCountMap_;
};

DataPoint& operator+=(DataPoint& lhs, const DataPoint& rhs);

/**
 * @brief Calculates the plots based on the currently selected plot type from all collected frame data sent to the model
 * @todo This is currently very slow: For small time steps, hundreds of data points are stored for a single point in the
 * plot
 */
class PlotModel : public QObject
{
    Q_OBJECT
public:
    PlotModel(QObject* parent = nullptr);

    /**
     * @brief Removes all plot data and emits an empty plot
     */
    void clear();

public slots:
    /**
     * @brief Extracts information from the `frameDTO` relevant for the plot as a `DataPoint` and averages all collected
     * `DataPoint`s if the plot time interval has elapsed since the last plot, emitting a new plot data point
     */
    void addDataPointFromFrameDTO(const FrameDTO& frameDTO);

signals:
    void dataPointAdded(const cell::DiscTypeMap<double>& dataPoint);
    void newPlotCreated(const QVector<cell::DiscTypeMap<double>>& dataPoints);

private:
    /**
     * @brief Emits the newest data point to be added to the plot
     */
    void emitDataPoint(const DataPoint& averagedDataPoint);

    /**
     * @brief Emits data for the full plot with all data points
     */
    void emitPlot();

    /**
     * @brief Turns a given FrameDTO into a DataPoint by summing up all relevant properties of all discs in the given
     * frame
     */
    DataPoint dataPointFromFrameDTO(const FrameDTO& frameDTO);

    /**
     * @brief
     */
    void plotAveragedDataPoint();

private:
    /**
     * @brief All data points received from the simulation
     */
    QVector<DataPoint> dataPoints_;

    /**
     * @brief If we collect all data points and average them all at once, visual stutter might be the result, so we
     * continously add data points to this one
     */
    DataPoint dataPointBeingAveraged_;

    /**
     * @brief Number of data points already added to the dataPointBeingAveraged_
     */
    int averagingCount_ = 0;

    /**
     * @brief Alias for frequently accessed setting
     */
    const sf::Time& PlotTimeInterval = GlobalGUISettings::getGUISettings().plotTimeInterval_;
};

#endif /* PLOTMODEL_HPP */
