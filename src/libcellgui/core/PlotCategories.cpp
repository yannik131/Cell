#include "PlotCategories.hpp"
#include "ColorMapping.hpp"
#include "Utility.hpp"

const QMap<PlotCategory, QString> PlotCategoryNameMapping{{PlotCategory::TotalCollisionCount, "Collision count"},
                                                          {PlotCategory::TotalAbsoluteImpulse, "Impulse"},
                                                          {PlotCategory::TotalKineticEnergy, "Kinetic energy"},
                                                          {PlotCategory::TypeCounts, "Type counts"},
                                                          {PlotCategory::VelocityDistribution, "Velocities"}};

const QMap<QString, PlotCategory> NamePlotCategoryMapping = Utility::invertMap(PlotCategoryNameMapping);

const QList<PlotCategory> SupportedPlotCategories = PlotCategoryNameMapping.keys();
const QStringList SupportedPlotCategoryNames = PlotCategoryNameMapping.values();