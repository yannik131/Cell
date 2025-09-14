#include "core/PlotCategories.hpp"
#include "core/ColorMapping.hpp"
#include "core/Utility.hpp"

const QMap<PlotCategory, QString> PlotCategoryNameMapping{{PlotCategory::TypeCounts, "Type counts"},
                                                          {PlotCategory::CollisionCounts, "Collision count"},
                                                          {PlotCategory::AbsoluteMomentum, "Impulse"},
                                                          {PlotCategory::KineticEnergy, "Kinetic energy"}};

const QList<PlotCategory> SupportedPlotCategories = PlotCategoryNameMapping.keys();
const QStringList SupportedPlotCategoryNames = PlotCategoryNameMapping.values();
