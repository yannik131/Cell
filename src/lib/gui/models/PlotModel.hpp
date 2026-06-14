#ifndef F981B716_35F7_4F94_A3F0_1AD44B1AE402_HPP
#define F981B716_35F7_4F94_A3F0_1AD44B1AE402_HPP

#include "cell/SimulationConfig.hpp"
#include "core/PlotCategories.hpp"
#include "core/Types.hpp"
#include "core/Utility.hpp"
#include "widgets/PlotWidget.hpp"

#include <QObject>

class Simulation;
using DataPoint = cell::DataPoint;
using Histogram = cell::Histogram;
using DiscTypeID = cell::DiscTypeID;

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
    void processDataPoint(const DataPoint& dataPoint);

    void setActivePlotDiscTypes(const std::vector<std::string>& activeDiscTypeNames);
    bool discTypeIsActiveInPlot(const std::string& discTypeName) const;

    const cell::DiscTypeRegistry& getDiscTypeRegistry() const;
    bool plotSum() const;

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

    void updatePlot(const DataPoint& dataPoint);
    void updateLinePlot();
    void updateHistogramPlot();
    void updateColorMapPlot();

    void updateLabelsAndColors();
    std::unordered_map<DiscTypeID, double> getActiveMap(const DataPoint& dataPoint);
    void updateActivePlotDiscTypes(const std::vector<cell::config::DiscType>& discTypes);
    Histogram sumHistogramStacks(const Histogram& histogram);
    Histogram discardInactiveDiscTypes(const Histogram& histogram);
    Histogram getVelocityHistogramFromDataPoint(const DataPoint& dataPoint, CalculateSum calculateSum);
    Histogram makeHistogramWithCategories(const Histogram& source, const std::vector<DiscTypeID>& categories);

    DataPoint createDataPoint() const;

private:
    Simulation* simulation_;

    ch::duration<double> plotTimeInterval_ = ch::duration<double>{0.1};
    bool plotSum_ = false;
    PlotCategory plotCategory_ = PlotCategory::TypeCounts;

    std::vector<std::string> labels_;
    std::vector<sf::Color> colors_;

    std::map<DiscTypeID, bool> activePlotDiscTypes_;
    DataPoint dataPoint_;
    double currentX_ = 0;
};

#endif /* F981B716_35F7_4F94_A3F0_1AD44B1AE402_HPP */
