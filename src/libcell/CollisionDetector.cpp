#include "CollisionDetector.hpp"
#include "ExceptionWithLocation.hpp"
#include "GlobalSettings.hpp"
#include "MathUtils.hpp"
#include "NanoflannAdapter.hpp"

#include <nanoflann.hpp>

#include <algorithm>

namespace cell
{

namespace
{
using AdapterType = nanoflann::L2_Simple_Adaptor<double, NanoflannAdapter>;
using KDTree = nanoflann::KDTreeSingleIndexAdaptor<AdapterType, NanoflannAdapter, 2>;
} // namespace

CollisionDetector::CollisionDetector()
{
    updateMaxDiscRadius();
}

void CollisionDetector::detectCollisions(std::vector<Disc>& discs, std::vector<Membrane>& membranes)
{
    discDiscCollisions_ = detectDiscDiscCollisions(discs);
    discMembraneCollisions_ = detectDiscMembraneCollisions(discs, membranes);
}

void CollisionDetector::updateMaxDiscRadius()
{
    const auto& discTypeDistribution = GlobalSettings::getSettings().discTypeDistribution_;
    if (discTypeDistribution.empty())
        throw ExceptionWithLocation("Disc type distribution can't be empty");

    maxDiscRadius_ = std::ranges::max_element(discTypeDistribution, [](const auto& a, const auto& b)
                                              { return a.first.getRadius() < b.first.getRadius(); })
                         ->first.getRadius();
}

double CollisionDetector::getMaxDiscRadius() const
{
    return maxDiscRadius_;
}

std::set<std::pair<Disc*, Disc*>> CollisionDetector::getDiscDiscCollisions() const
{
    return std::set<std::pair<Disc*, Disc*>>();
}

std::set<std::pair<Disc*, Membrane*>> CollisionDetector::getDiscMembraneCollisions() const
{
    return std::set<std::pair<Disc*, Membrane*>>();
}

std::set<std::pair<Disc*, Disc*>> CollisionDetector::detectDiscDiscCollisions(std::vector<Disc>& discs)
{
    NanoflannAdapter adapter(discs);
    KDTree kdtree(2, adapter);
    const nanoflann::SearchParameters searchParams(0, false);

    std::set<std::pair<Disc*, Disc*>> collidingDiscs;
    static std::vector<nanoflann::ResultItem<uint32_t, double>> discsInRadius;
    std::set<Disc*> discsInCollisions;

    for (auto& disc : discs)
    {
        // We do not support multiple simultaneous collisions
        if (disc.isMarkedDestroyed() || discsInCollisions.contains(&disc))
            continue;

        discsInRadius.clear();
        const double maxCollisionDistance = disc.getType().getRadius() + maxDiscRadius_;

        // This is the most time consuming part of the whole application, next to the index build in the KDTree
        // constructor
        kdtree.radiusSearch(&disc.getPosition().x, maxCollisionDistance * maxCollisionDistance, discsInRadius,
                            searchParams);

        for (const auto& result : discsInRadius)
        {
            auto& otherDisc = discs[result.first];
            if (&otherDisc == &disc || discsInCollisions.contains(&otherDisc))
                continue;

            const double radiusSum = disc.getType().getRadius() + otherDisc.getType().getRadius();

            if (result.second <= radiusSum * radiusSum)
            {
                const auto& pair = mathutils::makeOrderedPair(&disc, &otherDisc);
                collidingDiscs.insert(pair);

                discsInCollisions.insert(pair.first);
                discsInCollisions.insert(pair.second);

                break;
            }
        }
    }

    return collidingDiscs;
}

std::set<std::pair<Disc*, Membrane*>> CollisionDetector::detectDiscMembraneCollisions(std::vector<Disc>& discs,
                                                                                      std::vector<Membrane>& membranes)
{
    return std::set<std::pair<Disc*, Membrane*>>();
}

} // namespace cell