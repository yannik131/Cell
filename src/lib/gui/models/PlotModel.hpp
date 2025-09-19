#ifndef PLOTMODEL_HPP
#define PLOTMODEL_HPP

#include "cell/SimulationConfig.hpp"
#include "core/FrameDTO.hpp"
#include "core/PlotCategories.hpp"
#include "core/Types.hpp"

#include <QObject>

#include <SFML/System/Time.hpp>

/**
 * @brief Struct containing information from a FrameDTO relevant for the plot
 */
struct DataPoint
{
    double elapsedTime_ = 0;
    std::unordered_map<std::string, double> collisionCounts_;
    std::unordered_map<std::string, double> totalMomentumMap_;
    std::unordered_map<std::string, double> totalKineticEnergyMap_;
    std::unordered_map<std::string, double> discTypeCountMap_;
};

DataPoint& operator+=(DataPoint& lhs, const DataPoint& rhs);

class AbstractSimulationBuilder;

/**
 * @brief Calculates the plots based on the currently selected plot type from all collected frame data sent to the model
 * @todo This is currently very slow: For small time steps, hundreds of data points are stored for a single point in the
 * plot
 */
class PlotModel : public QObject
{
    Q_OBJECT
public:
    PlotModel(QObject* parent, AbstractSimulationBuilder* abstractSimulationBuilder);

    void setPlotCategory(PlotCategory plotCategory);
    void setPlotTimeInterval(int valueMilliseconds);
    void setPlotSum(bool value);
    void reset();

    void setActivePlotDiscTypes(const std::vector<std::string>& activeDiscTypeNames);
    const std::map<std::string, bool>& getActivePlotDiscTypesMap() const;

public slots:
    void processFrame(const FrameDTO& frameDTO);

signals:
    void createGraphs(const std::vector<std::string>& labels, const std::vector<sf::Color>& colors);
    void addDataPoint(const std::unordered_map<std::string, double>& dataPoint, double xStep, DoReplot doReplot);
    void addDataPoints(const std::vector<std::unordered_map<std::string, double>>& dataPoints, double xStep);
    void setPlotTitle(const std::string& title);

private:
    /**
     * @brief Emits data for the full plot with all data points
     */
    void emitPlot();

    /**
     * @brief Turns a given FrameDTO into a DataPoint by summing up all relevant properties of all discs in the given
     * frame
     */
    DataPoint dataPointFromFrameDTO(const FrameDTO& frameDTO);

    std::unordered_map<std::string, double> getActiveMap(const DataPoint& dataPoint);

    void storeDataPoint(const DataPoint& dataPoint);
    void plotDataPoint(const DataPoint& dataPoint);
    void emitGraphs();
    void updateActivePlotDiscTypes(const std::vector<cell::config::DiscType>& discTypes);

private:
    /**
     * @brief All data points received from the simulation
     */
    std::vector<DataPoint> dataPoints_;

    /**
     * @brief If we collect all data points and average them all at once, visual stutter might be the result, so we
     * continously add data points to this one
     */
    DataPoint dataPointForStorage_;
    DataPoint dataPointForPlotting_;

    /**
     * @brief Number of data points already added to the dataPointForStorage_
     */
    int averagingCount_ = 0;

    const double storageTime_ = 0.1;

    double plotTimeInterval_ = 0.1;
    bool plotSum_ = false;
    PlotCategory plotCategory_ = PlotCategory::CollisionCounts;

    AbstractSimulationBuilder* abstractSimulationBuilder_;

    std::vector<std::string> labels_;
    std::vector<sf::Color> colors_;

    std::map<std::string, bool> activePlotDiscTypes_;
};

#endif /* PLOTMODEL_HPP */
