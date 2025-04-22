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
    void addDataPoint(const QMap<DiscType, double>& dataPoint, bool doReplot = true);
    void replacePlot(const QVector<QMap<DiscType, double>>& dataPoints);
    void reset();

private:
    QCPTextElement* plotTitle_;

    double yMin_ = 0;
    double yMax_ = 0;

    double xMin_ = 0;
    double xMax_ = 0;

    QVector<QColor> colors_;
    QMap<DiscType, QCPGraph*> graphs_;
};

#endif