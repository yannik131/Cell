#ifndef PLOT_WIDGET_HPP
#define PLOT_WIDGET_HPP

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

    void createGraphs(const std::vector<std::string>& labels, const std::vector<sf::Color>& colors);

    void addDataPoint(const std::unordered_map<std::string, double>& dataPoint, double x, DoReplot = DoReplot{true});

    void replaceDataPoints(const std::vector<std::unordered_map<std::string, double>>& dataPoints, double xStep);

    void setPlotTitle(const std::string& title);

private:
    /**
     * @brief Create labels using correct units depending on the current plot type
     */
    void setAxisLabels();

    void reset();
    void resetRanges();
    void resetGraphs();

private:
    QCPTextElement* plotTitle_ = nullptr;

    double yMin_ = 0;
    double yMax_ = 0;

    double xMin_ = 0;
    double xMax_ = 0;

    std::unordered_map<std::string, QCPGraph*> graphs_;
};

#endif