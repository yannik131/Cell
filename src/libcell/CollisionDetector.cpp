#include "CollisionDetector.hpp"
#include "ExceptionWithLocation.hpp"
#include "GlobalSettings.hpp"
#include "MathUtils.hpp"
#include "Membrane.hpp"
#include "PositionNanoflannAdapter.hpp"

#include <algorithm>
#include <cmath>
#include <memory>
#include <numbers>

namespace cell
{

CollisionDetector::CollisionDetector()
    : membranesKDTreeAdapter_(membranePolygonPoints_)
    , membranesKDTree_(2, membranesKDTreeAdapter_)
{
    updateMaxRadii();
}

void CollisionDetector::buildMembraneKdTree(const std::vector<Membrane>& membranes)
{
    membranePolygonPoints_.reserve(membranes.size() * 100);
    membranePolygonPoints_.clear();

    for (const auto& membrane : membranes)
    {
        // We'll approximate the membrane circle with points on the circle that have a distance of the minimal disc
        // radius in the simulation
        const double R = membrane.getType()->getRadius();
        const double dphi = 2 * std::asin(minDiscRadius_ / (2 * R));

        for (double angle = 0; angle < 2 * std::numbers::pi; angle += dphi)
        {
            MembranePolygonPoint point{.membrane_ = &membrane, .position_ = {R * std::cos(angle), R * std::sin(angle)}};
            membranePolygonPoints_.push_back(std::move(point));
        }
    }

    membranesKDTree_.buildIndex();
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

    const double& R = disc.getType()->getRadius();
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
    const auto& compareRadius = [](const auto& a, const auto& b) { return a.getRadius() < b.getRadius(); };

    const auto& discTypes = GlobalSettings::getSettings().discTypes_;
    if (discTypes.empty())
        throw ExceptionWithLocation("Can't determine max disc type radius: No disc types available");
    maxDiscRadius_ = std::ranges::max_element(discTypes, compareRadius)->getRadius();
    minDiscRadius_ = std::ranges::min_element(discTypes, compareRadius)->getRadius();

    const auto& membraneTypes = GlobalSettings::getSettings().membraneTypes_;
    if (membraneTypes.empty())
        throw ExceptionWithLocation("Can't determine max membrane type radius: No membrane types available");
    maxMembraneRadius_ = std::ranges::max_element(membraneTypes, compareRadius)->getRadius();
}

double CollisionDetector::getMaxDiscRadius() const
{
    return maxDiscRadius_;
}

std::set<std::pair<Disc*, Disc*>> CollisionDetector::getDiscDiscCollisions() const
{
    return discDiscCollisions_;
}

std::set<std::pair<Disc*, Membrane*>> CollisionDetector::getDiscMembraneCollisions() const
{
    return discMembraneCollisions_;
}

std::set<std::pair<Disc*, Disc*>> CollisionDetector::detectDiscDiscCollisions(std::vector<Disc>& discs)
{
    PositionNanoflannAdapter<Disc> adapter(discs);
    KDTree<Disc> kdtree(2, adapter);

    // 0: Don't approximate neighbors during search
    // false: Don't sort results by distance

    static const nanoflann::SearchParameters searchParams(0, false);

    std::set<std::pair<Disc*, Disc*>> collidingDiscs;
    static std::vector<nanoflann::ResultItem<uint32_t, double>> discsInRadius;
    std::unordered_set<Disc*> discsInCollisions;

    for (auto& disc : discs)
    {
        // We do not support multiple simultaneous collisions
        if (disc.isMarkedDestroyed() || discsInCollisions.contains(&disc))
            continue;

        const double maxCollisionDistance = disc.getType()->getRadius() + maxDiscRadius_;

        // This is the most time consuming part of the whole application, next to the index build in the KDTree
        // constructor
        kdtree.radiusSearch(&disc.getPosition().x, maxCollisionDistance * maxCollisionDistance, discsInRadius,
                            searchParams);

        for (const auto& result : discsInRadius)
        {
            auto& otherDisc = discs[result.first];
            if (&otherDisc == &disc || discsInCollisions.contains(&otherDisc))
                continue;

            const double radiusSum = disc.getType()->getRadius() + otherDisc.getType()->getRadius();

            if (result.second <= radiusSum * radiusSum)
            {
                // Since all collisions are unique, the order doesn't really matter here
                collidingDiscs.insert({&disc, &otherDisc});
                discsInCollisions.insert(&disc);
                discsInCollisions.insert(&otherDisc);

                break;
            }
        }
    }

    return collidingDiscs;
}

std::set<std::pair<Disc*, Membrane*>> CollisionDetector::detectDiscMembraneCollisions(std::vector<Disc>& discs,
                                                                                      std::vector<Membrane>& membranes)
{
    std::set<std::pair<Disc*, Membrane*>> collisions;

    static std::vector<nanoflann::ResultItem<uint32_t, double>> polygonPointsInRadius;
    static const nanoflann::SearchParameters searchParams(0, false);

    for (auto& disc : discs)
    {
        if (disc.isMarkedDestroyed())
            continue;

        // TODO Make sure polygonPointsInRadius.clear() is actually called (but I think it is)
        membranesKDTree_.radiusSearch(&disc.getPosition().x, disc.getType()->getRadius() * disc.getType()->getRadius(),
                                      polygonPointsInRadius, searchParams);

        for (const auto& result : polygonPointsInRadius)
        {
            Membrane* membrane = &membranes[result.first];
            const double radiusSum = disc.getType()->getRadius() + membrane->getType()->getRadius();

            if (result.second <= radiusSum * radiusSum)
            {
                collisions.insert({&disc, membrane});
                break;
            }
        }
    }

    return collisions;
}

} // namespace cell