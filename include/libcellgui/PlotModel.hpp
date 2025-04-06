#ifndef PLOTMODEL_HPP
#define PLOTMODEL_HPP

#include "FrameDTO.hpp"
#include "PlotCategories.hpp"
#include "PlotData.hpp"

#include <QObject>

#include <SFML/System/Time.hpp>

/**
 * @brief Struct containing information from a FrameDTO relevant for the plot
 */
struct DataPoint
{
    long long elapsedTimeUs_ = 0;
    int collisionCount_ = 0;
    QMap<DiscType, double> totalMomentum_ = 0;
    QMap<DiscType, double> totalKineticEnergy_ = 0;
    QMap<DiscType, int> discTypeCount_;
};

template <typename T> QMap<DiscType, T> operator+=(QMap<DiscType, T>& a, const QMap<DiscType, T>& b)
{
    for (auto iter = a.begin(); iter != a.end(); ++iter)
        iter.value() += b[iter.key()];
}

template <typename T> QMap<DiscType, T> operator/=(QMap<DiscType, T>& a, const QMap<DiscType, T>& b)
{
    for (auto iter = a.begin(); iter != a.end(); ++iter)
        iter.value() /= b[iter.key()];
}

class PlotModel : public QObject
{
    Q_OBJECT
public:
    PlotModel(QObject* parent = nullptr);

    void setCurrentPlotCategory(const QString& plotCategoryName);

public slots:
    void receiveFrameDTO(const FrameDTO& frameDTO);
    void receiveSelectedDiscTypeNames(const QStringList& selectedDiscTypeNames);
    void plotTimeIntervalChanged(const sf::Time& plotTimeInterval);

signals:
    void dataPointAdded(const DataPoint& dataPoint, const PlotCategory& currentPlotCategory);
    void newPlotCreated(const QVector<DataPoint>& dataPoints, const PlotCategory& currentPlotCategory);

private:
    /**
     * @brief Emits the newest data point to be added to the plot
     */
    void emitDataPoint();

    /**
     * @brief Emits data for the full plot with all data points
     */
    void emitPlot();

private:
    /**
     * @brief Maps all DiscTypes to whether or not they're selected for the current plot
     */
    QMap<DiscType, bool> activeDiscTypesMap_;

    /**
     * @brief Recently received DataPoints where sum of DataPoint::elapsedTimeUs_ < plotTimeInterval_
     */
    QVector<DataPoint> dataPointsToAverage_;

    /**
     * @brief All data points received from the simulation
     */
    QVector<DataPoint> dataPoints_;

    /**
     * @brief Currently selected plot category in the UI
     */
    PlotCategory currentPlotCategory_;

    /**
     * @brief Accumulated time of the received FrameDTOs since the last data point was emitted
     */
    sf::Time elapsedWorldTimeSinceLastPlot_ = sf::Time::Zero;

    /**
     * @brief Copy of Settings::plotTimeInterval_
     */
    sf::Time plotTimeInterval_;
};

#endif /* PLOTMODEL_HPP */
