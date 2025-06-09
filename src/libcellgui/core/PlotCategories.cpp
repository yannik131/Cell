#include "PlotCategories.hpp"
#include "ColorMapping.hpp"
#include "Utility.hpp"

const QMap<PlotCategory, QString> PlotCategoryNameMapping{{PlotCategory::TypeCounts, "Type counts"},
                                                          {PlotCategory::CollisionCounts, "Collision count"},
                                                          {PlotCategory::AbsoluteImpulse, "Impulse"},
                                                          {PlotCategory::KineticEnergy, "Kinetic energy"}};

const QList<PlotCategory> SupportedPlotCategories = PlotCategoryNameMapping.keys();
const QStringList SupportedPlotCategoryNames = PlotCategoryNameMapping.values();
