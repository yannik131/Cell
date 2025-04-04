#ifndef PLOTDATA_HPP
#define PLOTDATA_HPP

#include "PlotCategories.hpp"

#include <QMap>
#include <QVector>

struct PlotData
{
    PlotCategory currentPlotCategory_;
    QVector<double> collisionCounts_;
    QVector<QMap<DiscType, int>> discTypeCounts_;
};

#endif /* PLOTDATA_HPP */
