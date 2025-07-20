#include "CollisionDetector.hpp"
#include "ExceptionWithLocation.hpp"
#include "GlobalSettings.hpp"
#include "MathUtils.hpp"
#include "PhysicalObjectNanoflannAdapter.hpp"

#include <nanoflann.hpp>

#include <algorithm>

namespace cell
{

namespace
{
template <typename T> using AdapterType = nanoflann::L2_Simple_Adaptor<double, PhysicalObjectNanoflannAdapter<T>>;

template <typename T>
using KDTree = nanoflann::KDTreeSingleIndexAdaptor<AdapterType<T>, PhysicalObjectNanoflannAdapter<T>, 2>;
} // namespace

CollisionDetector::CollisionDetector()
{
    updateMaxRadii();
}

void CollisionDetector::detectCollisions(std::vector<Disc>& discs, std::vector<Membrane>& membranes)
{
    discDiscCollisions_ = detectDiscDiscCollisions(discs);
    discMembraneCollisions_ = detectDiscMembraneCollisions(discs, membranes);
}

CollisionDetector::RectangleCollision CollisionDetector::detectDiscRectangleCollision(const Disc& disc,
                                                                                      const sf::Vector2d& topLeft,
                                                                                      const sf::Vector2d& bottomRight)
{
    using Wall = RectangleCollision::Wall;

    const double& R = disc.getType().getRadius();
    const sf::Vector2d& r = disc.getPosition();
    RectangleCollision rectangleCollision;
    double l;

    if (l = R + topLeft.x - r.x; l > 0)
        rectangleCollision.xCollision_ = {Wall::Left, l};
    else if (l = R - bottomRight.x + r.x; l > 0)
        rectangleCollision.xCollision_ = {Wall::Right, l};

    if (l = R + topLeft.y - r.y; l > 0)
        rectangleCollision.yCollision_ = {Wall::Top, l};
    else if (l = R - bottomRight.y + r.y; l > 0)
        rectangleCollision.yCollision_ = {Wall::Bottom, l};

    return rectangleCollision;
}

void CollisionDetector::updateMaxRadii()
{
    const auto& discTypeDistribution = GlobalSettings::getSettings().discTypeDistribution_;
    if (discTypeDistribution.empty())
        throw ExceptionWithLocation("Disc type distribution can't be empty");

    maxDiscRadius_ = std::ranges::max_element(discTypeDistribution, [](const auto& a, const auto& b)
                                              { return a.first.getRadius() < b.first.getRadius(); })
                         ->first.getRadius();

    const auto& membraneTypeDistribution = GlobalSettings::getSettings().membraneTypeDistribution_;
    if (membraneTypeDistribution.empty())
        throw ExceptionWithLocation("Membrane type distribution can't be empty");

    maxMembraneRadius_ = std::ranges::max_element(membraneTypeDistribution, [](const auto& a, const auto& b)
                                                  { return a.first.getRadius() < b.first.getRadius(); });
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
    PhysicalObjectNanoflannAdapter<Disc> adapter(discs);
    KDTree<Disc> kdtree(2, adapter);
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
    PhysicalObjectNanoflannAdapter<Membrane> adapter(membranes);
    KDTree<Membrane> kdtree(2, adapter);

    std::set<std::pair<Disc*, Membrane*>> collisions;

    static std::vector<nanoflann::ResultItem<uint32_t, double>> nearbyMembranes;
    const nanoflann::SearchParameters searchParams(0, false);

    for (auto& disc : discs)
    {
        if (disc.isMarkedDestroyed())
            continue;

        const double maxSearchRadius = maxMembraneRadius_ - disc.getType().getRadius();
    }
}

} // namespace cell