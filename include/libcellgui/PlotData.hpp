#ifndef PLOTDATA_HPP
#define PLOTDATA_HPP

#include "PlotCategories.hpp"

#include <QVector>

struct PlotData
{
    PlotCategory currentPlotCategory_;
    QVector<double> collisionCounts_;
};

#endif /* PLOTDATA_HPP */
