#include "core/PlotCategories.hpp"
#include "core/ColorMapping.hpp"
#include "core/Utility.hpp"

namespace PlotCategoryKeys
{
inline const QString TypeCounts = "Type counts";
inline const QString CollisionCounts = "Collision count";
inline const QString AbsoluteMomentum = "Absolute momentum";
inline const QString KineticEnergy = "Kinetic energy";
} // namespace PlotCategoryKeys

const QList<PlotCategory> SupportedPlotCategories{PlotCategory::TypeCounts, PlotCategory::CollisionCounts,
                                                  PlotCategory::AbsoluteMomentum, PlotCategory::KineticEnergy};

const QStringList SupportedPlotCategoryNames{PlotCategoryKeys::TypeCounts, PlotCategoryKeys::CollisionCounts,
                                             PlotCategoryKeys::AbsoluteMomentum, PlotCategoryKeys::KineticEnergy};

const QMap<QString, PlotCategory> PlotCategoryNameMapping{
    {PlotCategoryKeys::TypeCounts, PlotCategory::TypeCounts},
    {PlotCategoryKeys::CollisionCounts, PlotCategory::CollisionCounts},
    {PlotCategoryKeys::AbsoluteMomentum, PlotCategory::AbsoluteMomentum},
    {PlotCategoryKeys::KineticEnergy, PlotCategory::KineticEnergy}};
