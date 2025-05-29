#ifndef PLOT_WIDGET_HPP
#define PLOT_WIDGET_HPP

#include "PlotModel.hpp"

#include "qcustomplot.h"

#include <QColor>
#include <QWidget>

class PlotWidget : public QCustomPlot
{
    Q_OBJECT
public:
    explicit PlotWidget(QWidget* parent);

    void setModel(PlotModel* plotModel);
    void plotDataPoint(const DiscType::map<double>& dataPoint, bool doReplot = true);
    void replacePlot(const QVector<DiscType::map<double>>& dataPoints);
    void reset();

private:
    void addDataPointSum(const DiscType::map<double>& dataPoint);
    void addDataPoint(const DiscType::map<double>& dataPoint);

private:
    QCPTextElement* plotTitle_;

    double yMin_ = 0;
    double yMax_ = 0;

    double xMin_ = 0;
    double xMax_ = 0;

    QVector<QColor> colors_;
    DiscType::map<QCPGraph*> graphs_;
    QCPGraph* sumGraph_{};
};

#endif