#include "CollisionDetector.hpp"
#include "Disc.hpp"
#include "MathUtils.hpp"

#include <algorithm>

namespace cell
{
CollisionDetector::CollisionDetector(const DiscTypeRegistry& discTypeRegistry)
    : discTypeRegistry_(discTypeRegistry)
{
}

CollisionDetector::RectangleCollision
CollisionDetector::detectRectangularBoundsCollision(const Disc& disc, const sf::Vector2d& topLeft,
                                                    const sf::Vector2d& bottomRight) const
{
    using Wall = RectangleCollision::Wall;

    const double& R = discTypeRegistry_.getByID(disc.getDiscTypeID()).getRadius();
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

bool CollisionDetector::detectCircularBoundsCollision(const Disc& disc, const sf::Vector2d& M, double Rm) const
{
    const auto& r = disc.getPosition();
    const auto& Rc = discTypeRegistry_.getByID(disc.getDiscTypeID()).getRadius();

    return (M.x - r.x) * (M.x - r.x) + (M.y - r.y) * (M.y - r.y) >= (Rm - Rc) * (Rm - Rc);
}

std::vector<std::pair<Disc*, Disc*>> CollisionDetector::detectDiscDiscCollisions(std::vector<Disc>& discs)
{
    std::vector<std::pair<Disc*, Disc*>> collidingDiscs;
    collidingDiscs.reserve(discs.size() / 2);

    std::vector<char> discsInCollisions(discs.size(), 0);

    struct Entry
    {
        std::size_t index;
        double radius;
        sf::Vector2d position;
        double minX, maxX;
    };
    std::vector<Entry> entries;
    entries.reserve(discs.size());
    for (std::size_t i = 0; i < discs.size(); ++i)
    {
        Disc& d = discs[i];
        if (d.isMarkedDestroyed())
            continue;

        const double r = discTypeRegistry_.getByID(d.getDiscTypeID()).getRadius();
        const auto& p = d.getPosition();
        entries.push_back(Entry{.index = i, .radius = r, .position = p, .minX = p.x - r, .maxX = p.x + r});
    }

    std::sort(entries.begin(), entries.end(), [&](const Entry& e1, const Entry& e2) { return e1.minX < e2.minX; });

    for (std::size_t i = 0; i < entries.size(); ++i)
    {
        const auto& entry1 = entries[i];
        if (discsInCollisions[entry1.index])
            continue;

        for (std::size_t j = i + 1; j < entries.size(); ++j)
        {
            const auto& entry2 = entries[j];

            // sweep and prune: Since all elements are sorted by left x coordinate, if the current objects left x
            // coordinate is greater than the right x coordinate of the object we're comparing with, no other objects
            // can intersect with entry1 and we can stop
            if (entry2.minX > entry1.maxX)
                break;

            if (discsInCollisions[entry2.index])
                continue;

            if (mathutils::circlesIntersect(entry1.position, entry1.radius, entry2.position, entry2.radius))
            {
                auto d1 = &discs[entry1.index];
                auto d2 = &discs[entry2.index];

                collidingDiscs.emplace_back(d1, d2);
                collisionCounts_[d1->getDiscTypeID()]++;
                collisionCounts_[d2->getDiscTypeID()]++;

                discsInCollisions[entry1.index] = 1;
                discsInCollisions[entry2.index] = 1;
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