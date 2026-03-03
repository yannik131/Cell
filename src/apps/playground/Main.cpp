#include "qcustomplot.h"
#include <QDebug>
#include <boost/histogram.hpp>
#include <boost/histogram/indexed.hpp>
#include <iostream>
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

    for (int i = 0; i < labels.size() && i < colors.size(); ++i)
    {
        QCPBars* bar = new QCPBars(customPlot->xAxis, customPlot->yAxis);
        bar->setAntialiased(false);
        bar->setBrush(colors[i]);
        bar->setName(labels[i]);

        if (i > 0)
            bar->moveAbove(bars[i - 1]);

        bars.push_back(bar);
    }

    return bars;
}

void setData(const std::vector<QCPBars*>& bars, const Histogram& histogram, const QVector<double>& ticks)
{
    assert(bars.size() == histogram.axis(0).size());

    for (int i = 0; i < bars.size(); ++i)
    {
        QVector<double> counts;
        for (int j = 0; j < histogram.axis(1).size(); ++j)
            counts << histogram.at(i, j);

        bars[i]->setData(ticks, counts);
    }
}

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    QMainWindow window;
    QCustomPlot* customPlot = new QCustomPlot();
    window.setCentralWidget(customPlot);

    QVector<QString> discTypes = {"A", "B", "C"};
    const double mean = 50.0;
    const double sigma = 100.0;
    const int bins = 20;
    const auto h = createHistogram(mean, sigma, bins, discTypes.size());

    auto bars = createBars(discTypes, customPlot);

    QVector<double> ticks;
    QVector<QString> labels;

    for (int i = 0; i < bins; ++i)
    {
        ticks << i;
        labels << QString::number(h.axis(1).bin(i).center(), 'f', 1);
    }

    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTicks(ticks, labels);
    customPlot->xAxis->setTicker(textTicker);
    customPlot->xAxis->setRange(-1, bins);

    // prepare y axis:
    double max = 0;
    for (const auto& x : bh::indexed(h, bh::coverage::all))
        max = std::max(max, static_cast<double>(*x));

    customPlot->yAxis->setRange(0, discTypes.size() * max);
    customPlot->yAxis->setLabel("Counts");
    customPlot->yAxis->grid()->setSubGridVisible(true);

    // Add data:
    setData(bars, h, ticks);

    // setup legend:
    customPlot->legend->setVisible(true);
    customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop | Qt::AlignRight);
    customPlot->legend->setBrush(QColor(255, 255, 255, 100));
    customPlot->legend->setBorderPen(Qt::NoPen);
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    window.resize(600, 400);
    window.show();

    return app.exec();
}