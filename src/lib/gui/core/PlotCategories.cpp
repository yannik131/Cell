#include "core/PlotCategories.hpp"
#include "core/ColorMapping.hpp"
#include "core/Utility.hpp"

namespace PlotCategoryKeys
{
inline const QString TypeCounts = "Type counts";
inline const QString CollisionCounts = "Collision count";
inline const QString AbsoluteMomentum = "Absolute momentum";
inline const QString KineticEnergy = "Kinetic energy";
inline const QString VelocityDistribution = "Velocity distribution";
} // namespace PlotCategoryKeys

const QList<PlotCategory> SupportedPlotCategories{PlotCategory::TypeCounts, PlotCategory::CollisionCounts,
                                                  PlotCategory::AbsoluteMomentum, PlotCategory::KineticEnergy,
                                                  PlotCategory::VelocityDistribution};

const QStringList SupportedPlotCategoryNames{PlotCategoryKeys::TypeCounts, PlotCategoryKeys::CollisionCounts,
                                             PlotCategoryKeys::AbsoluteMomentum, PlotCategoryKeys::KineticEnergy,
                                             PlotCategoryKeys::VelocityDistribution};

const QMap<QString, PlotCategory> PlotCategoryNameMapping{
    {PlotCategoryKeys::TypeCounts, PlotCategory::TypeCounts},
    {PlotCategoryKeys::CollisionCounts, PlotCategory::CollisionCounts},
    {PlotCategoryKeys::AbsoluteMomentum, PlotCategory::AbsoluteMomentum},
    {PlotCategoryKeys::KineticEnergy, PlotCategory::KineticEnergy},
    {PlotCategoryKeys::VelocityDistribution, PlotCategory::VelocityDistribution}};
