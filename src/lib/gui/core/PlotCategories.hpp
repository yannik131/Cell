#ifndef D902B588_844F_4F0F_8893_E0001BB5976E_HPP
#define D902B588_844F_4F0F_8893_E0001BB5976E_HPP

#include <QMap>
#include <QString>

/**
 * @brief Currently supported plot types by the GUI
 */
enum class PlotCategory
{
    TypeCounts,
    CollisionCounts,
    AbsoluteMomentum,
    KineticEnergy,
    XVelocityDistribution,
    YVelocityDistribution,
    AbsoluteVelocityDistribution,
    VelocityColorMap
};

const QVector<PlotCategory>& SupportedPlotCategories();

const QStringList& SupportedPlotCategoryNames();

const QMap<QString, PlotCategory>& PlotCategoryNameMap();

#endif /* D902B588_844F_4F0F_8893_E0001BB5976E_HPP */
