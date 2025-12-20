#include "core/PlotCategories.hpp"
#include "core/ColorMapping.hpp"
#include "core/Utility.hpp"

namespace PlotCategoryKeys
{
inline const QString TypeCounts = "Type counts";
inline const QString CollisionCounts = "Collision count";
inline const QString Impulse = "Impulse";
inline const QString KineticEnergy = "Kinetic energy";
} // namespace PlotCategoryKeys

const QStringList SupportedPlotCategoryNames{PlotCategoryKeys::TypeCounts, PlotCategoryKeys::CollisionCounts,
                                             PlotCategoryKeys::Impulse, PlotCategoryKeys::KineticEnergy};

const QMap<QString, PlotCategory> PlotCategoryNameMapping{
    {PlotCategoryKeys::TypeCounts, PlotCategory::TypeCounts},
    {PlotCategoryKeys::CollisionCounts, PlotCategory::CollisionCounts},
    {PlotCategoryKeys::Impulse, PlotCategory::AbsoluteMomentum},
    {PlotCategoryKeys::KineticEnergy, PlotCategory::KineticEnergy}};
