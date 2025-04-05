#ifndef ANALYSIS_PLOT_WIDGET_HPP
#define ANALYSIS_PLOT_WIDGET_HPP

#include "PlotModel.hpp"

#include "qcustomplot.h"

#include <QColor>
#include <QWidget>

class AnalysisPlotWidget : public QCustomPlot
{
    Q_OBJECT
public:
    explicit AnalysisPlotWidget(QWidget* parent);

    void reset();
    void plot(const PlotData& plotData);
    void setModel(PlotModel* plotModel);

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