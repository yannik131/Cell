#ifndef ANALYSIS_PLOT_WIDGET_HPP
#define ANALYSIS_PLOT_WIDGET_HPP

#include "PlotDataModel.hpp"

#include "qcustomplot.h"

#include <QWidget>

class AnalysisPlotWidget : public QCustomPlot
{
    Q_OBJECT
public:
    explicit AnalysisPlotWidget(QWidget* parent);

    void reset();

public:
    void plot(const PlotData& plotData);
    PlotDataModel* plotDataModel() const;

private:
    QVector<double> xData_;
    QVector<double> yData_;
    QCPTextElement* plotTitle_;

    double yMin_ = 0;
    double yMax_ = 0;

    PlotDataModel* plotDataModel_;
};

#endif