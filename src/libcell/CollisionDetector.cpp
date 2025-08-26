#include "CollisionDetector.hpp"
#include "Disc.hpp"
#include "PositionNanoflannAdapter.hpp"

namespace cell
{
CollisionDetector::CollisionDetector(DiscTypeResolver discTypeResolver, MaxRadiusProvider maxRadiusProvider)
    : discTypeResolver_(std::move(discTypeResolver))
    , maxRadiusProvider_(std::move(maxRadiusProvider))
{
}

void cell::CollisionDetector::detectCollisions(std::vector<Disc>& discs)
{
    discDiscCollisions_ = detectDiscDiscCollisions(discs);
}

CollisionDetector::RectangleCollision
CollisionDetector::detectDiscRectangleCollision(const Disc& disc, const sf::Vector2d& topLeft,
                                                const sf::Vector2d& bottomRight) const
{
    using Wall = RectangleCollision::Wall;

    const double& R = discTypeResolver_(disc.getDiscTypeID()).getRadius();
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

std::set<std::pair<Disc*, Disc*>> CollisionDetector::getDiscDiscCollisions() const
{
    return discDiscCollisions_;
}

std::set<std::pair<Disc*, Disc*>> CollisionDetector::detectDiscDiscCollisions(std::vector<Disc>& discs) const
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

        const double maxCollisionDistance = discTypeResolver_(disc.getDiscTypeID()).getRadius() + maxRadiusProvider_();

        // This is the most time consuming part of the whole application, next to the index build in the KDTree
        // constructor
        kdtree.radiusSearch(&disc.getPosition().x, maxCollisionDistance * maxCollisionDistance, discsInRadius,
                            searchParams);

        for (const auto& result : discsInRadius)
        {
            auto& otherDisc = discs[result.first];
            if (&otherDisc == &disc || discsInCollisions.contains(&otherDisc))
                continue;

            const double radiusSum = discTypeResolver_(disc.getDiscTypeID()).getRadius() +
                                     discTypeResolver_(otherDisc.getDiscTypeID()).getRadius();

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

} // namespace cell