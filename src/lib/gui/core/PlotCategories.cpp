#include "core/PlotCategories.hpp"
#include "core/ColorMapping.hpp"
#include "core/Utility.hpp"

const QMap<QString, PlotCategory> PlotCategoryNameMapping{{"Type counts", PlotCategory::TypeCounts},
                                                          {"Collision count", PlotCategory::CollisionCounts},
                                                          {"Impulse", PlotCategory::AbsoluteMomentum},
                                                          {"Kinetic energy", PlotCategory::KineticEnergy}};

const QStringList SupportedPlotCategoryNames = PlotCategoryNameMapping.keys();
