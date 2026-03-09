#ifndef F981B716_35F7_4F94_A3F0_1AD44B1AE402_HPP
#define F981B716_35F7_4F94_A3F0_1AD44B1AE402_HPP

#include "cell/SimulationConfig.hpp"
#include "core/FrameDTO.hpp"
#include "core/PlotCategories.hpp"
#include "core/Types.hpp"
#include "core/Utility.hpp"

#include <QObject>
#include <QVector>
#include <boost/histogram.hpp>
#include <optional>

namespace bh = boost::histogram;

using Histogram = bh::histogram<std::tuple<bh::axis::category<std::string>, bh::axis::regular<>>, bh::default_storage>;
using HeatMapColumn = QVector<double>;
using HeatMapData = QVector<HeatMapColumn>;

/**
 * @brief Struct containing information from a FrameDTO relevant for the plot
 */
struct DataPoint
{
    DataPoint(double vxSigma, std::vector<std::string> discTypes)
        : vxHistogram_(bh::make_histogram(bh::axis::category<std::string>(std::move(discTypes), "Disc types"),
                                          bh::axis::regular<>(20, -3 * vxSigma, 3 * vxSigma, "v.x")))
    {
    }

    explicit DataPoint(const cell::SimulationConfig& config)
        : DataPoint(config.mostProbableSpeed, utility::extract(config.discTypes, &cell::config::DiscType::name))
    {
    }

    void clear()
    {
        elapsedTime_ = 0;
        collisionCounts_.clear();
        totalMomentumMap_.clear();
        totalKineticEnergyMap_.clear();
        discTypeCountMap_.clear();
        vxHistogram_.reset();
    }

    double elapsedTime_ = 0;
    std::unordered_map<std::string, double> collisionCounts_;
    std::unordered_map<std::string, double> totalMomentumMap_;
    std::unordered_map<std::string, double> totalKineticEnergyMap_;
    std::unordered_map<std::string, double> discTypeCountMap_;
    Histogram vxHistogram_;
};

DataPoint& operator+=(DataPoint& lhs, const DataPoint& rhs);

class Simulation;

/**
 * @brief Calculates the plots based on the currently selected plot type from all collected frame data sent to the model
 */
class PlotModel : public QObject
{
    Q_OBJECT
public:
    PlotModel(QObject* parent, Simulation* simulation);

    void setPlotCategory(PlotCategory plotCategory);
    void setPlotTimeInterval(int valueMilliseconds);
    void setPlotSum(bool value);
    void reset();

    void setActivePlotDiscTypes(const std::vector<std::string>& activeDiscTypeNames);
    const std::map<std::string, bool>& getActivePlotDiscTypesMap() const;

public slots:
    void processFrame(const FrameDTO& frameDTO);

signals:
    void createLinePlots(const std::vector<std::string>& labels, const std::vector<sf::Color>& colors);
    void createHistogram(const std::vector<std::string>& labels, const std::vector<sf::Color>& colors,
                         const Histogram& histogram);
    void createHeatMap(double xMin, double xMax, double yMin, double yMax, int xCells, int yCells);

    void linePlotPoint(const std::unordered_map<std::string, double>& dataPoint, double xStep,
                       DoReplot = DoReplot{true});
    void linePlotPoints(const std::vector<std::unordered_map<std::string, double>>& dataPoints, double xStep);
    void histogram(const Histogram& histogram);
    void heatMap(const HeatMapData& columns);

    void plotTitle(const std::string& title);

private:
    void emitWholePlot();
    void emitLinePlot();
    void emitHistogram();
    void emitHeatMap();

    void emitPlotPart();
    void emitLinePlotPoints();
    void emitHeatMapColumn();
    void emitGraphs();

    void updateActivePlotDiscTypes(const std::vector<cell::config::DiscType>& discTypes);

    DataPoint dataPointFromFrameDTO(const FrameDTO& frameDTO);
    std::unordered_map<std::string, double> getActiveMap(const DataPoint& dataPoint);
    void storeDataPoint(const DataPoint& dataPoint);

    Histogram sumHistogramStacks(const Histogram& histogram) const;
    Histogram discardInactiveDiscTypes(const Histogram& histogram) const;
    Histogram makeHistogramWithCategories(const Histogram& source, const std::vector<std::string>& categories) const;
    Histogram makeActiveHeatMapHistogram(const Histogram& histogram) const;

    HeatMapColumn makeHeatMapColumn(const Histogram& histogram) const;
    HeatMapData buildWholeHeatMap() const;
    void captureInitialHistogram(const FrameDTO& frameDTO);

private:
    Simulation* simulation_ = nullptr;

    PlotCategory plotCategory_ = PlotCategory::TypeCounts;
    bool plotSum_ = false;

    std::vector<std::unordered_map<std::string, double>> plotData_;
    std::vector<DataPoint> dataPoints_;
    std::map<std::string, bool> activePlotDiscTypes_;

    DataPoint dataPointForStorage_;
    DataPoint dataPointForPlotting_;

    std::optional<Histogram> initialVxHistogram_;

    std::vector<std::string> labels_;
    std::vector<sf::Color> colors_;

    int averagingCount_ = 0;
    double plotTimeInterval_ = 0.1;
    const double storageTime_ = 0.1;
};

#endif /* F981B716_35F7_4F94_A3F0_1AD44B1AE402_HPP */