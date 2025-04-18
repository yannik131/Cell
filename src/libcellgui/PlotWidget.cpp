#include "PlotWidget.hpp"
#include "GlobalGUISettings.hpp"
#include "GlobalSettingsFunctor.hpp"

#include <algorithm>

PlotWidget::PlotWidget(QWidget* parent)
    : QCustomPlot(parent)
{
    setInteraction(QCP::iRangeDrag, true); // Allow dragging the plot by left click-hold
    setInteraction(QCP::iRangeZoom, true); // Allow zoom with mouse wheel

    plotTitle_ =
        new QCPTextElement(this, PlotCategoryNameMapping[GlobalGUISettings::getGUISettings().currentPlotCategory_],
                           QFont("sans", 12, QFont::Bold));
    plotLayout()->insertRow(0);
    plotLayout()->addElement(0, 0, plotTitle_);

    xAxis->setLabel("t [s]");
    yAxis->setLabel("N");

    addLayer("legend layer");
    legend->setLayer("legend layer");
    legend->setVisible(true);

    // Place the legend
    axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom | Qt::AlignRight);

    // These are the default colors for matplotlib. I like them.
    colors_ << QColor(31, 119, 180) << QColor(255, 127, 14) << QColor(44, 160, 44) << QColor(214, 39, 40)
            << QColor(148, 103, 189) << QColor(140, 86, 75) << QColor(227, 119, 194) << QColor(127, 127, 127)
            << QColor(188, 189, 34) << QColor(23, 190, 207);

    reset();
}

void PlotWidget::reset()
{
    xMin_ = 0;
    xMax_ = 0;
    yMin_ = 0;
    yMax_ = 0;

    clearGraphs();
    graphs_.clear();

    plotTitle_->setText(PlotCategoryNameMapping[GlobalGUISettings::getGUISettings().currentPlotCategory_]);

    const auto& discTypesPlotMap = GlobalGUISettings::getGUISettings().discTypesPlotMap_;
    for (auto iter = discTypesPlotMap.begin(); iter != discTypesPlotMap.end(); ++iter)
    {
        if (!iter.value())
            continue;

        QCPGraph* graph = addGraph();
        graph->setPen(colors_[graphs_.size() % colors_.size()]);
        graph->setName(QString::fromStdString(iter.key().getName()));
        graphs_[iter.key()] = graph;
    }

    for (int i = 0; i < legend->itemCount(); ++i)
        legend->item(i)->setLayer("legend layer");
}

void PlotWidget::replacePlot(const QVector<QMap<DiscType, double>>& dataPoints)
{
    reset();
    for (const auto& dataPoint : dataPoints)
        addDataPoint(dataPoint);
}

void PlotWidget::addDataPoint(const QMap<DiscType, double>& dataPoint)
{
    if (graphs_.empty())
        return;

    auto size = graphs_.first()->dataCount();
    auto timeStep = GlobalGUISettings::getGUISettings().plotTimeInterval_.asMilliseconds();
    double sum = 0.0;

    for (auto iter = dataPoint.begin(); iter != dataPoint.end(); ++iter)
    {
        // Skip disabled disc types
        if (!graphs_.contains(iter.key()))
            continue;

        xMax_ = timeStep * size;
        graphs_[iter.key()]->addData(xMax_, iter.value());

        yMin_ = std::min(yMin_, iter.value());
        yMax_ = std::max(yMax_, iter.value());

        sum += iter.value();
    }

    yAxis->setRange(yMin_, yMax_);
    xAxis->setRange(xMin_, xMax_);

    plotTitle_->setText("Sum: " + QString::number(sum));

    replot();
}

void PlotWidget::setModel(PlotModel* plotModel)
{
    connect(plotModel, &PlotModel::dataPointAdded, this, &PlotWidget::addDataPoint);
    connect(plotModel, &PlotModel::newPlotCreated, this, &PlotWidget::replacePlot);
}
