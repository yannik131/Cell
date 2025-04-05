#ifndef PLOTMODEL_HPP
#define PLOTMODEL_HPP

#include "FrameDTO.hpp"
#include "PlotCategories.hpp"
#include "PlotData.hpp"

#include <QObject>

#include <SFML/System/Time.hpp>

struct Frame
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
    void plotIntervalChanged();

signals:
    void plotDataPoint(const Frame& plotData, const PlotCategory& currentPlotCategory);
    void fullPlot(const QVector<Frame>& plotData, const PlotCategory& currentPlotCategory);

private:
    /**
     * @brief Emits the newest data point to be added to the plot
     */
    void emitPoint();

    /**
     * @brief Emits data for the full plot with all data points
     */
    void emitFullPlot();

private:
    QMap<DiscType, bool> activeDiscTypesMap_;
    QVector<Frame> framesToAverage_;
    QVector<Frame> frames_;
    QVector<Frame> averagedFrames_;
    PlotCategory currentPlotCategory_;
    sf::Time elapsedWorldTimeSinceLastPlot_ = sf::Time::Zero;
    const sf::Time& plotInterval_;
};

#endif /* PLOTMODEL_HPP */
