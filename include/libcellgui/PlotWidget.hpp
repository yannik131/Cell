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

private:
    void clear();
    void addDataPoint(const DataPoint& plotData);

private:
    QCPTextElement* plotTitle_;

    double yMin_ = 0;
    double yMax_ = 0;

    double xMin_ = 0;
    double xMax_ = 0;

    QVector<QColor> colors_;
};

#endif