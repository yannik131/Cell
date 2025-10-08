#include "CollisionDetector.hpp"
#include "Disc.hpp"
#include "MathUtils.hpp"

#include <algorithm>

namespace cell
{
CollisionDetector::CollisionDetector(const DiscTypeRegistry& discTypeRegistry,
                                     const MembraneTypeRegistry& membraneTypeRegistry)
    : discTypeRegistry_(discTypeRegistry)
    , membraneTypeRegistry_(membraneTypeRegistry)
{
}

CollisionDetector::RectangleCollision
CollisionDetector::detectRectangularBoundsCollision(const Disc& disc, const sf::Vector2d& topLeft,
                                                    const sf::Vector2d& bottomRight) const
{
    using Wall = RectangleCollision::Wall;

    const double& R = discTypeRegistry_.getByID(disc.getTypeID()).getRadius();
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
    const auto& Rc = discTypeRegistry_.getByID(disc.getTypeID()).getRadius();

    return (M.x - r.x) * (M.x - r.x) + (M.y - r.y) * (M.y - r.y) >= (Rm - Rc) * (Rm - Rc);
}

void CollisionDetector::buildDiscEntries(const std::vector<Disc>& discs)
{
    discEntries_.clear();
    discEntries_.reserve(discs.size());

    // Collision detection happens before reactions -> no destroyed check necessary
    for (std::size_t i = 0; i < discs.size(); ++i)
        discEntries_.push_back(createEntry(discs[i], discTypeRegistry_, i));

    std::sort(discEntries_.begin(), discEntries_.end(),
              [&](const Entry& e1, const Entry& e2) { return e1.minX < e2.minX; });
}

void CollisionDetector::buildForeignEntries(const std::vector<Membrane>& membranes,
                                            const std::vector<Disc*>& intrudingDiscs)
{
    foreignEntries_.clear();
    foreignEntries_.reserve(membranes.size() + intrudingDiscs.size());

    for (std::size_t i = 0; i < membranes.size(); ++i)
        foreignEntries_.push_back(
            ForeignEntry{createEntry(membranes[i], membraneTypeRegistry_, i), .type_ = EntryType::Membrane});

    for (std::size_t i = 0; i < intrudingDiscs.size(); ++i)
        foreignEntries_.push_back(
            ForeignEntry{createEntry(*intrudingDiscs[i], discTypeRegistry_, i), .type_ = EntryType::IntrudingDisc});

    std::sort(foreignEntries_.begin(), foreignEntries_.end(),
              [&](const Entry& e1, const Entry& e2) { return e1.minX < e2.minX; });
}

std::vector<std::pair<Disc*, Disc*>> CollisionDetector::detectDiscDiscCollisions(std::vector<Disc>& discs)
{
    std::vector<std::pair<Disc*, Disc*>> collisions;
    collisions.reserve(discs.size() / 2);

    std::vector<char> discsInCollisions(discs.size(), 0);

    for (std::size_t i = 0; i < discEntries_.size(); ++i)
    {
        const auto& entry1 = discEntries_[i];
        if (discsInCollisions[entry1.index])
            continue;

        for (std::size_t j = i + 1; j < discEntries_.size(); ++j)
        {
            const auto& entry2 = discEntries_[j];

            // sweep and prune: Since all elements are sorted by left x coordinate, if the current objects left x
            // coordinate is greater than the right x coordinate of the object we're comparing with, no other objects
            // can intersect with entry1 and we can stop
            if (entry2.minX > entry1.maxX)
                break;

            if (discsInCollisions[entry2.index])
                continue;

            if (mathutils::circlesOverlap(entry1.position, entry1.radius, entry2.position, entry2.radius))
            {
                auto d1 = &discs[entry1.index];
                auto d2 = &discs[entry2.index];

                collisions.emplace_back(d1, d2);
                collisionCounts_[d1->getTypeID()]++;
                collisionCounts_[d2->getTypeID()]++;

                discsInCollisions[entry1.index] = 1;
                discsInCollisions[entry2.index] = 1;
                break;
            }
        }
    }

    return collisions;
}

std::vector<std::pair<Membrane*, Disc*>>
CollisionDetector::detectMembraneDiscCollisions(std::vector<Membrane>& membranes, std::vector<Disc>& discs)
{
    std::vector<std::pair<Membrane*, Disc*>> collisions;

    std::size_t startIndex = 0;
    for (const auto& membraneEntry : membraneEntries_)
    {
        while (startIndex < discEntries_.size())
        {
            if (discEntries_[startIndex].maxX >= membraneEntry.minX)
                break;

            ++startIndex;
        }

        for (std::size_t i = startIndex; i < discEntries_.size(); ++i)
        {
            const auto& discEntry = discEntries_[i];

            if (discEntry.minX > membraneEntry.maxX)
                break;

            if (mathutils::circlesIntersect(membraneEntry.position, membraneEntry.radius, discEntry.position,
                                            discEntry.radius))
            {
                auto m = &membranes[membraneEntry.index];
                auto d = &discs[discEntry.index];

                collisions.emplace_back(m, d);
                break;
            }
        }
    }

    return collisions;
}

DiscTypeMap<int> CollisionDetector::getAndResetCollisionCounts()
{
    auto tmp = std::move(collisionCounts_);
    collisionCounts_.clear();

    return tmp;
}

} // namespace cell