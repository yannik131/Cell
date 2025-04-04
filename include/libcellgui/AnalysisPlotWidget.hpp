#ifndef ANALYSIS_PLOT_WIDGET_HPP
#define ANALYSIS_PLOT_WIDGET_HPP

#include "PlotDataModel.hpp"

#include "qcustomplot.h"

#include <QColor>
#include <QWidget>

class AnalysisPlotWidget : public QCustomPlot
{
    Q_OBJECT
public:
    explicit AnalysisPlotWidget(QWidget* parent);

    void reset();

public:
    void plot(const PlotData& plotData);

private:
    void plotCollisionCount(const PlotData& plotData);
    void plotDiscTypeCounts(const PlotData& plotData);

private:
    QCPTextElement* plotTitle_;

    double yMin_ = 0;
    double yMax_ = 0;

    double xMin_ = 0;
    double xMax_ = 0;

    QVector<QColor> colors_;
};

#endif