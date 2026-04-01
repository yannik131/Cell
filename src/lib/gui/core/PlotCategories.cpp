#include "core/PlotCategories.hpp"
#include "core/ColorMapping.hpp"
#include "core/Utility.hpp"

namespace
{

using Pair = std::pair<PlotCategory, QString>;

const std::vector<Pair> PlotCategoryPairs{
    {PlotCategory::TypeCounts, "Type counts"},
    {PlotCategory::CollisionCounts, "Collision counts"},
    {PlotCategory::AbsoluteMomentum, "Absolute momentum"},
    {PlotCategory::KineticEnergy, "Kinetic energy"},
    {PlotCategory::VelocityDistribution, "Velocity distribution"},
    {PlotCategory::VelocityColorMap, "Velocity color map"},
};

struct PlotCategoryCache
{
    QVector<PlotCategory> categories;
    QStringList names;
    QMap<QString, PlotCategory> nameToCategory;
};

const PlotCategoryCache& cache()
{
    static const PlotCategoryCache c = []
    {
        PlotCategoryCache out;
        out.categories.reserve(PlotCategoryPairs.size());
        out.names.reserve(PlotCategoryPairs.size());

        for (const auto& [category, name] : PlotCategoryPairs)
        {
            out.categories.push_back(category);
            out.names.push_back(name);
            out.nameToCategory.insert(name, category);
        }
        return out;
    }();

    return c;
}

} // namespace

const QVector<PlotCategory>& SupportedPlotCategories()
{
    return cache().categories;
}

const QStringList& SupportedPlotCategoryNames()
{
    return cache().names;
}

const QMap<QString, PlotCategory>& PlotCategoryNameMap()
{
    return cache().nameToCategory;
}