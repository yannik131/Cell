#ifndef ANALYSIS_PLOT_HPP
#define ANALYSIS_PLOT_HPP

#include "qcustomplot.h"

#include <QWidget>

class AnalysisPlot : public QCustomPlot
{
    Q_OBJECT
public:
    explicit AnalysisPlot(QWidget* parent);

    void addDataPoint(double y);

private:
    QVector<double> xData_;
    QVector<double> yData_;
    QCPTextElement* plotTitle_;

    double yMin_ = 0;
    double yMax_ = 0;
};

#endif