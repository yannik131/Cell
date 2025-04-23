#ifndef PLOTCATEGORIES_HPP
#define PLOTCATEGORIES_HPP

#include <QMap>
#include <QString>

enum PlotCategory
{
    TotalCollisionCount,
    TotalAbsoluteImpulse,
    TotalKineticEnergy,
    TypeCounts
};

extern const QMap<PlotCategory, QString> PlotCategoryNameMapping;
extern const QMap<QString, PlotCategory> NamePlotCategoryMapping;

extern const QList<PlotCategory> SupportedPlotCategories;
extern const QStringList SupportedPlotCategoryNames;

#endif /* PLOTCATEGORIES_HPP */
