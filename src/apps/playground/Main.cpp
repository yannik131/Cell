#include "qcustomplot.h"
#include <QApplication>
#include <QMainWindow>
#include <QTimer>
#include <algorithm>
#include <array>
#include <random>
#include <vector>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    QMainWindow window;
    auto* customPlot = new QCustomPlot();
    window.setCentralWidget(customPlot);
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    QCPColorMap* colorMap = new QCPColorMap(customPlot->xAxis, customPlot->yAxis);
    std::vector<std::vector<int>> histograms;
    auto* timer = new QTimer(customPlot);
    auto addHistogram = [&]()
    {
        std::vector<int> histogram;
        for (int i = 0; i < 5; ++i)
            histogram.push_back(i * (histograms.size() + 1));
        histograms.push_back(histogram);
        colorMap->data()->setSize(histograms.size(), 5);
        for (int i = 0; i < histograms.size(); ++i)
        {
            for (int j = 0; j < 5; ++j)
                colorMap->data()->setCell(i, j, histograms[i][j]);
        }
        if (histograms.size() == 1)
            colorMap->data()->setRange(QCPRange(0, 1), QCPRange(0.5, 4.5));
        else
            colorMap->data()->setRange(QCPRange(0.5, histograms.size() - 0.5), QCPRange(0.5, 4.5));
        colorMap->rescaleDataRange();
        customPlot->yAxis->setRange(0, 5);
        customPlot->xAxis->setRange(0, histograms.size());
        customPlot->replot(QCustomPlot::rpQueuedReplot);
    };
    QObject::connect(timer, &QTimer::timeout, addHistogram);
    QCPColorScale* colorScale = new QCPColorScale(customPlot);
    customPlot->plotLayout()->addElement(0, 1, colorScale);
    colorScale->setType(QCPAxis::atRight);
    colorMap->setColorScale(colorScale);
    colorMap->setGradient(QCPColorGradient::gpGrayscale);
    // colorMap->setInterpolate(false);
    auto ticker = QSharedPointer<QCPAxisTickerText>::create();
    for (int i = 0; i < 5; ++i)
        ticker->addTick(i + 0.5, QString("v") + QString::number(i));
    customPlot->yAxis->setTicker(ticker);
    addHistogram();
    timer->start(100);
    window.resize(700, 400);
    window.show();
    return app.exec();
}