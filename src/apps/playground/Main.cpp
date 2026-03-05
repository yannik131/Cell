#include "qcustomplot.h"
#include <boost/histogram.hpp>
#include <boost/histogram/indexed.hpp>
#include <random>
#include <vector>

namespace bh = boost::histogram;

using Histogram = bh::histogram<std::tuple<bh::axis::category<QString>, bh::axis::regular<>>, bh::default_storage>;

Histogram createHistogram(double mean, double sigma, int bins, const QVector<QString>& categories)
{
    auto h = bh::make_histogram(bh::axis::category<QString>(categories, "Disc types"),
                                bh::axis::regular<>(bins, mean - 2 * sigma, mean + 2 * sigma, "v.x"));

    std::random_device rd{};
    std::mt19937 gen(rd());
    std::normal_distribution<double> vDist(mean, sigma);
    std::uniform_int_distribution<int> dDist(0, categories.size() - 1);

    for (int i = 0; i < 10000; ++i)
        h(categories[dDist(gen)], vDist(gen));

    return h;
}

std::unordered_map<QString, QCPBars*> createBars(const QVector<QString>& labels, QCustomPlot* customPlot)
{
    std::vector<QColor> colors = {QColor(Qt::red),    QColor(Qt::green),   QColor(Qt::blue),
                                  QColor(Qt::yellow), QColor(Qt::magenta), QColor(Qt::cyan)};
    std::unordered_map<QString, QCPBars*> bars;

    for (int i = 0; i < labels.size() && i < (int)colors.size(); ++i)
    {
        auto* bar = new QCPBars(customPlot->xAxis, customPlot->yAxis);
        bar->setAntialiased(false);
        bar->setBrush(colors[i]);
        bar->setName(labels[i]);
        bar->setPen(QPen(Qt::black, 1));
        bar->setStackingGap(0);

        // stacked bars:
        if (i > 0)
            bar->moveAbove(bars[labels[i - 1]]);

        bars[labels[i]] = bar;
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

void setDataAndWidths(const std::unordered_map<QString, QCPBars*>& bars, const Histogram& h)
{
    auto const& catAx = h.axis<0>(); // axis::category<QString>
    auto const& regAx = h.axis<1>(); // axis::regular<>

    Q_ASSERT((int)bars.size() == (int)catAx.size());

    const double binW = regAx.bin(0).width();
    const QVector<double> x = binCenters(h);

    for (auto& [label, bar] : bars)
    {
        bar->setWidth(binW);
    }

    for (int i = 0; i < (int)catAx.size(); ++i)
    {
        const QString label = catAx.value(i); // <-- QString, not double

        QVector<double> counts;
        counts.reserve((int)regAx.size());
        for (int j = 0; j < (int)regAx.size(); ++j)
            counts << h.at(i, j); // indices

        bars.at(label)->setData(x, counts); // const map => use at()
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

    const auto h = createHistogram(mean, sigma, bins, discTypes);
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