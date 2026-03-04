#include "qcustomplot.h"
#include <boost/histogram.hpp>
#include <boost/histogram/indexed.hpp>
#include <random>
#include <vector>

namespace bh = boost::histogram;

using Histogram = bh::histogram<std::tuple<bh::axis::integer<int>, bh::axis::regular<>>, bh::default_storage>;

Histogram createHistogram(double mean, double sigma, int bins, int dimension)
{
    auto h = bh::make_histogram(bh::axis::integer<int>(0, dimension, "Disc types"),
                                bh::axis::regular<>(bins, mean - 2 * sigma, mean + 2 * sigma, "v.x"));

    std::random_device rd{};
    std::mt19937 gen(rd());
    std::normal_distribution<double> vDist(mean, sigma);
    std::uniform_int_distribution<int> dDist(0, dimension - 1);

    for (int i = 0; i < 10000; ++i)
        h(dDist(gen), vDist(gen));

    return h;
}

std::vector<QCPBars*> createBars(const QVector<QString>& labels, QCustomPlot* customPlot)
{
    std::vector<QColor> colors = {QColor(Qt::red),    QColor(Qt::green),   QColor(Qt::blue),
                                  QColor(Qt::yellow), QColor(Qt::magenta), QColor(Qt::cyan)};
    std::vector<QCPBars*> bars;

    for (int i = 0; i < labels.size() && i < (int)colors.size(); ++i)
    {
        auto* bar = new QCPBars(customPlot->xAxis, customPlot->yAxis);
        bar->setAntialiased(false);
        bar->setBrush(colors[i]);
        bar->setName(labels[i]);
        bar->setPen(QPen(Qt::black, 1));

        // stacked bars:
        if (i > 0)
            bar->moveAbove(bars[i - 1]);

        bars.push_back(bar);
    }

    return bars;
}

static QVector<double> binCenters(const Histogram& h)
{
    const auto& ax = h.axis(1);
    QVector<double> x;
    x.reserve((int)ax.size());
    for (int i = 0; i < (int)ax.size(); ++i)
        x << ax.bin(i).center();
    return x;
}

void setDataAndWidths(const std::vector<QCPBars*>& bars, const Histogram& h)
{
    Q_ASSERT((int)bars.size() == (int)h.axis(0).size());

    const auto& ax = h.axis(1);
    const double binW = ax.bin(0).width(); // regular axis => constant width
    const QVector<double> x = binCenters(h);

    for (auto* b : bars)
    {
        b->setWidthType(QCPBars::wtPlotCoords);
        b->setWidth(binW); // fill the bin range in plot coordinates
    }

    for (int i = 0; i < (int)bars.size(); ++i)
    {
        QVector<double> counts;
        counts.reserve((int)ax.size());
        for (int j = 0; j < (int)ax.size(); ++j)
            counts << h.at(i, j);

        bars[i]->setData(x, counts);
    }
}

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    QMainWindow window;
    auto* customPlot = new QCustomPlot();
    window.setCentralWidget(customPlot);

    QVector<QString> discTypes = {"A", "B", "C"};
    const double mean = 50.0;
    const double sigma = 100.0;
    const int bins = 30;

    const auto h = createHistogram(mean, sigma, bins, discTypes.size());
    auto bars = createBars(discTypes, customPlot);

    // Let QCustomPlot manage ticks (default ticker is numeric/automatic).
    // Just set a sensible numeric range matching the histogram axis:
    const auto& ax = h.axis(1);
    const double xMin = ax.value(0);
    const double xMax = ax.value(ax.size());
    customPlot->xAxis->setRange(xMin, xMax);
    customPlot->xAxis->setLabel(ax.metadata().empty() ? "x" : ax.metadata().c_str());
    customPlot->xAxis->setNumberFormat("f");  // fixed-point
    customPlot->xAxis->setNumberPrecision(0); // 2 decimals

    QCPRange r = customPlot->xAxis->range();
    const double bw = ax.bin(0).width();
    const double origin = ax.value(0);
    double approxStep = r.size() / 8.0; // want ~8 ticks
    int k = std::max(1, (int)std::round(approxStep / bw));

    auto t = QSharedPointer<QCPAxisTickerFixed>::create();
    t->setTickOrigin(origin);
    t->setTickStep(k * bw);
    customPlot->xAxis->setTicker(t);

    // Prepare y axis:
    double maxBinCount = 0.0;
    for (const auto& x : bh::indexed(h, bh::coverage::all))
        maxBinCount = std::max(maxBinCount, static_cast<double>(*x));

    customPlot->yAxis->setRange(0, discTypes.size() * maxBinCount);
    customPlot->yAxis->setLabel("Counts");
    customPlot->yAxis->grid()->setSubGridVisible(true);

    // Add data (and make bars fill bin widths):
    setDataAndWidths(bars, h);

    // Legend & interactions:
    customPlot->legend->setVisible(true);
    customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop | Qt::AlignRight);
    customPlot->legend->setBrush(QColor(255, 255, 255, 100));
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    window.resize(600, 400);
    window.show();

    return app.exec();
}