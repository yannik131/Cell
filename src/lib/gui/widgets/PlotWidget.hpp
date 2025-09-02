#ifndef PLOT_WIDGET_HPP
#define PLOT_WIDGET_HPP

#include "models/PlotModel.hpp"

#include "qcustomplot.h"

#include <QColor>
#include <QWidget>

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

    /**
     * @brief Adds a single data point to the plot
     * @param dataPoint A map containing a value for each disc type. What this value represents does not matter in this
     * context and depends on the currently selected `PlotCategory`
     * @param doReplot If `true`, redraw the plot visually. Set this to false if a lot of data points are to be added to
     * the plot in a short time to avoid updating it unnecessarily often
     */
    void plotDataPoint(const cell::DiscTypeMap<double>& dataPoint, bool doReplot = true);

    /**
     * @brief Deletes the old plot and plots the given dataPoints
     */
    void replacePlot(const QVector<cell::DiscTypeMap<double>>& dataPoints);

    /**
     * @brief Deletes the current plot and creates either an empty sum plot graph or empty graphs for each disc type
     * based on the current plot data selection
     */
    void reset();

private:
    /**
     * @brief Given a data point, add a new point to the plot displaying the sum across all disc types
     */
    void addDataPointSum(const cell::DiscTypeMap<double>& dataPoint);

    /**
     * @brief Given a data point, add a new point for each disc type to the respective graph with the given value
     */
    void addDataPoint(const cell::DiscTypeMap<double>& dataPoint);

    /**
     * @brief Creates an empty, single sum graph, updating title and legend accordingly
     */
    void createSumGraph();

    /**
     * @brief Creates empty graphs for all disc types enabled for plotting, updating title and legend accordingly
     */
    void createRegularGraphs();

    /**
     * @brief Create labels using correct units depending on the current plot type
     */
    void setAxisLabels();

private:
    QCPTextElement* plotTitle_;

    double yMin_ = 0;
    double yMax_ = 0;

    double xMin_ = 0;
    double xMax_ = 0;

    QVector<QColor> colors_;
    cell::DiscTypeMap<QCPGraph*> graphs_;
    QCPGraph* sumGraph_{};
};

#endif