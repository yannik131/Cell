#ifndef EF3DF9A2_5589_4286_A435_DAC8EC61FB2F_HPP
#define EF3DF9A2_5589_4286_A435_DAC8EC61FB2F_HPP

#include "core/PlotCategories.hpp"
#include "core/Types.hpp"

#include <SFML/Graphics/Color.hpp>
#include <qcustomplot.h>

class PlotModel;
using DiscTypeID = cell::DiscTypeID;
using Histogram = cell::Histogram;

class PlotWidget : public QCustomPlot
{
    Q_OBJECT
public:
    struct LinePlotParams
    {
        const std::vector<std::string>& labels;
        const std::vector<sf::Color>& colors;
        const std::vector<std::unordered_map<DiscTypeID, double>>& dataPoints;
        const std::vector<double>& x;
        const PlotCategory& plotCategory;
    };

    struct LinePlotData
    {
        const std::unordered_map<DiscTypeID, double>& dataPoint;
        bool doReplot;
        double x;
    };

    struct HistogramParams
    {
        const std::vector<std::string>& labels;
        const std::vector<sf::Color>& colors;
        const Histogram& histogram;
    };

    struct HistogramData
    {
        const Histogram& histogram;
    };

    struct ColorMapParams
    {
        const std::vector<Histogram>& histograms;
        double xStep;
    };

    struct ColorMapData
    {
        const Histogram& histogram;
        double xStep;
    };

public:
    explicit PlotWidget(QWidget* parent);

    void setModel(PlotModel* plotModel);

    void setPlot(const LinePlotParams& linePlotParams);
    void setPlot(const HistogramParams& histogramParams);
    void setPlot(const ColorMapParams& colorMapParams);

    void updatePlot(const LinePlotData& linePlotData);
    void updatePlot(const HistogramData& histogramData);
    void updatePlot(const ColorMapData& colorMapData);

    void setInterpolate(bool enabled);
    const cell::DiscTypeRegistry& getDiscTypeRegistry() const;

    void startPlotTimer();
    void stopPlotTimer();

private:
    void clear();
    void clearRanges();
    void clearGraphs();
    void updateLegend(const std::vector<std::string>& labels);
    void enableZoom(bool enabled);
    void setHistogramYRange(double yMax);
    QString getYAxisLabelFromPlotCategory(const PlotCategory& plotCategory) const;
    void replotIfNewPlotAvailable();

private:
    double yMin_ = 0;
    double yMax_ = 0;
    double xMin_ = 0;
    double xMax_ = 0;

    std::unordered_map<std::string, QCPGraph*> graphs_;
    std::unordered_map<std::string, QCPBars*> histogram_;
    QCPColorMap* colorMap_ = nullptr;
    QCPColorScale* colorScale_ = nullptr;

    std::vector<Histogram> colorMapCache_;

    bool interpolateEnabled_ = false;

    std::function<const cell::DiscTypeRegistry&()> discTypeRegistryProvider_;
    std::function<bool()> plotSumProvider_;

    QTimer plotTimer_;
    bool newPlotAvailable_ = false;
};

#endif /* EF3DF9A2_5589_4286_A435_DAC8EC61FB2F_HPP */
