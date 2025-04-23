#ifndef PLOTCATEGORIES_HPP
#define PLOTCATEGORIES_HPP

#include <QMap>
#include <QString>

enum PlotCategory
{
    TypeCounts,
    TotalCollisionCount,
    TotalAbsoluteImpulse,
    TotalKineticEnergy
};

extern const QMap<PlotCategory, QString> PlotCategoryNameMapping;
extern const QMap<QString, PlotCategory> NamePlotCategoryMapping;

extern const QList<PlotCategory> SupportedPlotCategories;
extern const QStringList SupportedPlotCategoryNames;

#endif /* PLOTCATEGORIES_HPP */
