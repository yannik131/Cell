#include "AnalysisPlotWidget.hpp"
#include "GlobalSettings.hpp"

#include <algorithm>

AnalysisPlotWidget::AnalysisPlotWidget(QWidget* parent)
    : QCustomPlot(parent)
{
    setInteraction(QCP::iRangeDrag, true); // Allow dragging the plot by left click-hold
    setInteraction(QCP::iRangeZoom, true); // Allow zoom with mouse wheel

    plotTitle_ = new QCPTextElement(this, "Collisions per second", QFont("sans", 12, QFont::Bold));
    plotLayout()->insertRow(0);
    plotLayout()->addElement(0, 0, plotTitle_);

    xAxis->setLabel("t [s]");
    yAxis->setLabel("N");

    addLayer("legend layer");
    legend->setLayer("legend layer");
    legend->setVisible(true);

    // Put legend in right center
    axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom | Qt::AlignRight);

    // These are the default colors for matplotlib. I like them.
    colors_ << QColor(31, 119, 180) << QColor(255, 127, 14) << QColor(44, 160, 44) << QColor(214, 39, 40)
            << QColor(148, 103, 189) << QColor(140, 86, 75) << QColor(227, 119, 194) << QColor(127, 127, 127)
            << QColor(188, 189, 34) << QColor(23, 190, 207);
}

void AnalysisPlotWidget::reset()
{
    xMin_ = 0;
    xMax_ = 0;
    yMin_ = 0;
    yMax_ = 0;

    clearGraphs();
    replot();
}

void AnalysisPlotWidget::plot(const PlotData& plotData)
{
    reset();

    switch (plotData.currentPlotCategory_)
    {
    case PlotCategory::TotalCollisionCount: plotCollisionCount(plotData); break;
    case PlotCategory::TypeCounts: plotDiscTypeCounts(plotData); break;
    }

    yAxis->setRange(yMin_, yMax_);
    xAxis->setRange(xMin_, xMax_);

    for (int i = 0; i < legend->itemCount(); ++i)
        legend->item(i)->setLayer("legend layer");

    replot();
}

void AnalysisPlotWidget::setModel(PlotModel* plotModel)
{
    connect(plotModel, &PlotModel::plotDataPoint, this, &AnalysisPlotWidget::plotDataPoint);
    connect(plotModel, &PlotModel::fullPlot, this, &AnalysisPlotWidget::fullPlot);
}

void AnalysisPlotWidget::plotCollisionCount(const PlotData& plotData)
{
    if (plotData.collisionCounts_.empty())
        return;

    QVector<double> x;
    for (int i = 0; i < plotData.collisionCounts_.size(); ++i)
        x.push_back(i * GlobalSettings::getSettings().plotTimeInterval_.asSeconds());

    xMin_ = 0;
    xMax_ = x.back();

    yMin_ = 0;
    yMax_ = *std::max_element(plotData.collisionCounts_.begin(), plotData.collisionCounts_.end());

    QCPGraph* graph = addGraph();
    graph->setPen(QPen(QColor(31, 119, 180)));
    graph->setData(x, plotData.collisionCounts_, true);
}

void AnalysisPlotWidget::plotDiscTypeCounts(const PlotData& plotData)
{
    if (plotData.discTypeCount_.empty())
        return;

    QVector<double> x;
    for (int i = 0; i < plotData.discTypeCount_.size(); ++i)
        x.push_back(i * GlobalSettings::getSettings().plotTimeInterval_.asSeconds());

    xMin_ = 0;
    xMax_ = x.back();

    yMin_ = 0;

    QMap<DiscType, QCPGraph*> graphs;

    for (auto iter = plotData.discTypeCount_.first().begin(); iter != plotData.discTypeCount_.first().end(); ++iter)
    {
        int index = std::distance(plotData.discTypeCount_.first().begin(), iter);
        QCPGraph* graph = addGraph();

        graph->setName(QString::fromStdString(iter.key().name_));
        graph->setPen(QPen(colors_[index]));
        graphs[iter.key()] = graph;
    }

    for (int i = 0; i < plotData.discTypeCount_.size(); ++i)
    {
        for (auto iter = plotData.discTypeCount_[i].begin(); iter != plotData.discTypeCount_[i].end(); ++iter)
        {
            graphs[iter.key()]->addData(x[i], iter.value());
            if (iter.value() > yMax_)
                yMax_ = iter.value();
        }
    }
}
