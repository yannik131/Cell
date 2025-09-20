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

CollisionDetector::RectangleCollision
CollisionDetector::detectDiscRectangleCollision(const Disc& disc, const sf::Vector2d& topLeft,
                                                const sf::Vector2d& bottomRight) const
{
    using Wall = RectangleCollision::Wall;

    const double& R = discTypeResolver_(disc.getDiscTypeID()).getRadius();
    const sf::Vector2d& r = disc.getPosition();
    RectangleCollision rectangleCollision;
    double l = NAN;

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

std::unordered_set<std::pair<Disc*, Disc*>, PairHasher>
CollisionDetector::detectDiscDiscCollisions(std::vector<Disc>& discs)
{
    PositionNanoflannAdapter<Disc> adapter(discs);
    KDTree<Disc> kdtree(2, adapter);

    // 0: Don't approximate neighbors during search
    // false: Don't sort results by distance

    static const nanoflann::SearchParameters searchParams(0, false);
    static std::vector<nanoflann::ResultItem<uint32_t, double>> discsInRadius;
    std::unordered_set<std::pair<Disc*, Disc*>, PairHasher> collidingDiscs;
    std::vector<char> discsInCollisions(discs.size(), 0);

    for (std::size_t i = 0; i < discs.size(); ++i)
    {
        auto& disc = discs[i];
        // We do not support multiple simultaneous collisions
        if (disc.isMarkedDestroyed() || discsInCollisions[i])
            continue;

        const double maxCollisionDistance = discTypeResolver_(disc.getDiscTypeID()).getRadius() + maxRadiusProvider_();

        // This is the most time consuming part of the whole application, next to the index build in the KDTree
        // constructor
        kdtree.radiusSearch(&disc.getPosition().x, maxCollisionDistance * maxCollisionDistance, discsInRadius,
                            searchParams);

        for (const auto& result : discsInRadius)
        {
            auto& otherDisc = discs[result.first];
            if (&otherDisc == &disc || discsInCollisions[result.first])
                continue;

            const double radiusSum = discTypeResolver_(disc.getDiscTypeID()).getRadius() +
                                     discTypeResolver_(otherDisc.getDiscTypeID()).getRadius();

            if (result.second <= radiusSum * radiusSum)
            {
                // Since all collisions are unique, the order doesn't really matter here
                collidingDiscs.insert({&disc, &otherDisc});
                collisionCounts_[disc.getDiscTypeID()]++;
                collisionCounts_[otherDisc.getDiscTypeID()]++;

                discsInCollisions[i] = 1;
                discsInCollisions[result.first] = 1;

                break;
            }
        }
    }

    return collidingDiscs;
}

DiscTypeMap<int> CollisionDetector::getAndResetCollisionCounts()
{
    auto tmp = std::move(collisionCounts_);
    collisionCounts_.clear();

    return tmp;
}

} // namespace cell