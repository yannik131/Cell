#ifndef F981B716_35F7_4F94_A3F0_1AD44B1AE402_HPP
#define F981B716_35F7_4F94_A3F0_1AD44B1AE402_HPP

#include "cell/SimulationConfig.hpp"
#include "core/PlotCategories.hpp"
#include "core/Types.hpp"
#include "core/Utility.hpp"
#include "widgets/PlotWidget.hpp"

#include <QObject>

struct DataPoint
{
    DataPoint(double vxSigma, std::vector<std::string> discTypes)
        : vxHistogram_(bh::make_histogram(bh::axis::category<std::string>(std::move(discTypes), "Disc types"),
                                          bh::axis::regular<>(20, -3 * vxSigma, 3 * vxSigma, "v_x")))
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
    // TODO Don't use simulation here?
    PlotModel(QObject* parent, Simulation* simulation);

    void setPlotCategory(PlotCategory plotCategory);
    void setPlotTimeInterval(int valueMilliseconds);
    void setPlotSum(bool value);
    void setInterpolate(bool value);
    void reset();

    void setActivePlotDiscTypes(const std::vector<std::string>& activeDiscTypeNames);
    const std::map<std::string, bool>& getActivePlotDiscTypesMap() const;

signals:
    void setPlot(const PlotWidget::LinePlotParams& linePlotParams);
    void setPlot(const PlotWidget::HistogramParams& histogramParams);
    void setPlot(const PlotWidget::ColorMapParams& colorMapParams);

    void updatePlot(const PlotWidget::LinePlotData& linePlotData);
    void updatePlot(const PlotWidget::HistogramData& histogramData);
    void updatePlot(const PlotWidget::ColorMapData& colorMapData);

    void interpolateEnabled(bool enabled);

private:
    void setPlot();
    void setLinePlot();
    void setHistogramPlot();
    void setColorMapPlot();

    void updatePlot();
    void updateLinePlot();
    void updateHistogramPlot();
    void updateColorMapPlot();

    void updateLabelsAndColors();
    std::unordered_map<std::string, double> getActiveMap(const DataPoint& dataPoint);
    void storeDataPoint(const DataPoint& dataPoint);
    void updateActivePlotDiscTypes(const std::vector<cell::config::DiscType>& discTypes);
    Histogram sumHistogramStacks(const Histogram& histogram);
    Histogram discardInactiveDiscTypes(const Histogram& histogram);
    Histogram getVelocityHistogramFromDataPoint(const DataPoint& dataPoint, CalculateSum calculateSum);
    Histogram makeHistogramWithCategories(const Histogram& source, const std::vector<std::string>& categories);

private:
    std::vector<DataPoint> dataPoints_;
    Simulation* simulation_;

    DataPoint dataPointForStorage_;
    DataPoint dataPointForPlotting_;

    const double storageTime_ = 0.1;

    double plotTimeInterval_ = 0.1;
    bool plotSum_ = false;
    PlotCategory plotCategory_ = PlotCategory::TypeCounts;

    std::vector<std::string> labels_;
    std::vector<sf::Color> colors_;

    std::map<std::string, bool> activePlotDiscTypes_;
};

#endif /* F981B716_35F7_4F94_A3F0_1AD44B1AE402_HPP */
