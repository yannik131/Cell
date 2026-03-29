#ifndef EF3DF9A2_5589_4286_A435_DAC8EC61FB2F_HPP
#define EF3DF9A2_5589_4286_A435_DAC8EC61FB2F_HPP

#include "core/Types.hpp"
#include "models/PlotModel.hpp"

#include "qcustomplot.h"

/**
 * @brief Widget containing the plot displaying information on the simulation
 */
class PlotWidget : public QCustomPlot
{
    Q_OBJECT
public:
    explicit PlotWidget(QWidget* parent);

    /**
     * @brief Connects callbacks to the given model
     */
    void setModel(PlotModel* plotModel);

    void createLinePlots(const std::vector<std::string>& labels, const std::vector<sf::Color>& colors);
    void createHistogram(const std::vector<std::string>& labels, const std::vector<sf::Color>& colors,
                         const Histogram& histogram);

    void plotLinePlotPoint(const std::unordered_map<std::string, double>& dataPoint, double xStep,
                           DoReplot = DoReplot{true});
    void plotLinePlotPoints(const std::vector<std::unordered_map<std::string, double>>& dataPoints, double xStep);
    void plotHistogram(const Histogram& histogram);

    void setPlotTitle(const std::string& title);

private:
    void reset();
    void resetRanges();
    void resetGraphs();
    void updateLegend(const std::vector<std::string>& labels);
    void enableZoom(bool enabled);
    void setHistogramYRange(double yMax);

private:
    QCPTextElement* plotTitle_ = nullptr;

    double yMin_ = 0;
    double yMax_ = 0;

    double xMin_ = 0;
    double xMax_ = 0;

    std::unordered_map<std::string, QCPGraph*> graphs_;
    std::unordered_map<std::string, QCPBars*> histogram_;

    int count_ = 0;
};

#endif /* EF3DF9A2_5589_4286_A435_DAC8EC61FB2F_HPP */
