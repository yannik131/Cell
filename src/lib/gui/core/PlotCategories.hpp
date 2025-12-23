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
    KineticEnergy
};

/**
 * This is similar to the color mapping, but since we don't depend on other non-local static data, no singletons are
 * necessary to avoid SIOF
 */

/**
 * @brief Maps the plot types to human readable strings for the GUI
 */
extern const QMap<QString, PlotCategory> PlotCategoryNameMapping;

/**
 * @brief Contains all entries of `PlotCategory` as a list
 */
extern const QList<PlotCategory> SupportedPlotCategories;

/**
 * @brief Contains all entries of `PlotCategory` as a list of human readable strings
 */
extern const QStringList SupportedPlotCategoryNames;

#endif /* D902B588_844F_4F0F_8893_E0001BB5976E_HPP */
