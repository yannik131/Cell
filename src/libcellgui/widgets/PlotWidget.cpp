#include "PlotWidget.hpp"
#include "GlobalGUISettings.hpp"
#include "GlobalSettingsFunctor.hpp"
#include "Utility.hpp"

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
    axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop | Qt::AlignCenter);

    // These are the default colors for matplotlib. I like them.
    colors_ << QColor(31, 119, 180) << QColor(255, 127, 14) << QColor(44, 160, 44) << QColor(214, 39, 40)
            << QColor(148, 103, 189) << QColor(140, 86, 75) << QColor(227, 119, 194) << QColor(127, 127, 127)
            << QColor(188, 189, 34) << QColor(23, 190, 207);

    reset();
}

void PlotWidget::reset()
{
    xMin_ = 0;
    xMax_ = INT_MIN;
    yMin_ = INT_MAX;
    yMax_ = INT_MIN;

    clearGraphs();
    graphs_.clear();
    sumGraph_ = nullptr;

    // TODO This is really ugly because we could be in 2 different states (plotting sums or not) which need to be
    // handled differently but also share some things

    if (GlobalGUISettings::getGUISettings().plotSum_)
    {
        sumGraph_ = addGraph();
        sumGraph_->setPen(QColor());
        legend->setVisible(false);
        plotTitle_->setText(PlotCategoryNameMapping[GlobalGUISettings::getGUISettings().currentPlotCategory_] +
                            " (sum)");

        return;
    }

    legend->setVisible(true);
    plotTitle_->setText(PlotCategoryNameMapping[GlobalGUISettings::getGUISettings().currentPlotCategory_]);

    const auto& discTypesPlotMap = GlobalGUISettings::getGUISettings().discTypesPlotMap_;
    for (const auto& [discType, plotEnabled] : discTypesPlotMap)
    {
        if (!plotEnabled)
            continue;

        QCPGraph* graph = addGraph();
        graph->setPen(Utility::sfColorToQColor(discType.getColor()));
        graph->setName(QString::fromStdString(discType.getName()));
        graphs_[discType] = graph;
    }

    for (int i = 0; i < legend->itemCount(); ++i)
        legend->item(i)->setLayer("legend layer");
}

void PlotWidget::replacePlot(const QVector<DiscType::map<double>>& dataPoints)
{
    reset();

    if (dataPoints.empty())
    {
        replot();
        return;
    }

    for (auto i = 0; i < dataPoints.size(); ++i)
        plotDataPoint(dataPoints[i], i == dataPoints.size() - 1);
}

void PlotWidget::plotDataPoint(const DiscType::map<double>& dataPoint, bool doReplot)
{
    if (GlobalGUISettings::getGUISettings().plotSum_)
        addDataPointSum(dataPoint);
    else
        addDataPoint(dataPoint);

    if (doReplot)
    {
        yAxis->setRange(yMin_, yMax_);
        xAxis->setRange(xMin_, xMax_);

        replot();
    }
}

void PlotWidget::addDataPoint(const DiscType::map<double>& dataPoint)
{
    if (graphs_.empty())
        return;

    const auto& size = graphs_.begin()->second->dataCount();
    const auto& timeStep = GlobalGUISettings::getGUISettings().plotTimeInterval_.asSeconds();

    for (const auto& [discType, value] : dataPoint)
    {
        if (!GlobalGUISettings::getGUISettings().discTypesPlotMap_.at(discType))
            continue;

        xMax_ = timeStep * static_cast<float>(size);
        graphs_[discType]->addData(xMax_, value);

        yMin_ = std::min(yMin_, value);
        yMax_ = std::max(yMax_, value);
    }
}

void PlotWidget::addDataPointSum(const DiscType::map<double>& dataPoint)
{
    if (sumGraph_ == nullptr)
        throw ExceptionWithLocation(
            "Can't add data point to sumGraph_: Is nullptr (this is a bug and shouldn't happen");

    const auto& size = sumGraph_->dataCount();
    const auto& timeStep = GlobalGUISettings::getGUISettings().plotTimeInterval_.asSeconds();
    double sum = 0.0;

    for (const auto& [discType, value] : dataPoint)
    {
        if (!GlobalGUISettings::getGUISettings().discTypesPlotMap_.at(discType))
            continue;

        xMax_ = timeStep * static_cast<float>(size);
        sum += value;
    }

    sumGraph_->addData(xMax_, sum);
    yMin_ = std::min(yMin_, sum);
    yMax_ = std::max(yMax_, sum);
}

void PlotWidget::setModel(PlotModel* plotModel)
{
    connect(plotModel, &PlotModel::dataPointAdded,
            [this](const DiscType::map<double>& dataPoint) { plotDataPoint(dataPoint); });
    connect(plotModel, &PlotModel::newPlotCreated, this, &PlotWidget::replacePlot);
}
