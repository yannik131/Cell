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

void CollisionDetector::buildEntries(const std::vector<Disc>& discs, const std::vector<Membrane>& membranes,
                                     const std::vector<Disc*>& intrudingDiscs)
{
    entries_.clear();
    entries_.reserve(discs.size() + membranes.size() + intrudingDiscs.size());

    for (std::size_t i = 0; i < discs.size(); ++i)
        entries_.push_back(createEntry(discs[i], discTypeRegistry_, i, EntryType::Disc));

    for (std::size_t i = 0; i < membranes.size(); ++i)
        entries_.push_back(createEntry(membranes[i], membraneTypeRegistry_, i, EntryType::Membrane));

    for (std::size_t i = 0; i < intrudingDiscs.size(); ++i)
        entries_.push_back(createEntry(*intrudingDiscs[i], discTypeRegistry_, i, EntryType::IntrudingDisc));

    std::sort(entries_.begin(), entries_.end(), [&](const Entry& e1, const Entry& e2) { return e1.minX < e2.minX; });
}

CollisionDetector::Collisions CollisionDetector::detectCollisions(std::vector<Disc>* discs,
                                                                  std::vector<Membrane>* membranes,
                                                                  std::vector<Disc*>* intrudingDiscs)
{
    Collisions collisions;
    collisions.discDiscCollisions.reserve(discs->size() / 2);

    std::vector<char> discsInCollisions(discs->size(), 0);
    std::vector<char> intrudingDiscsInCollisions(intrudingDiscs ? intrudingDiscs->size() : 0, 0);

    const auto isInCollision = [&](const Entry& entry)
    {
        return (entry.type == EntryType::Disc && discsInCollisions[entry.index]) ||
               (entry.type == EntryType::IntrudingDisc && intrudingDiscsInCollisions[entry.index]);
    };

    for (std::size_t i = 0; i < entries_.size(); ++i)
    {
        const auto& entry1 = entries_[i];
        if (isInCollision(entry1))
            continue;

        for (std::size_t j = i + 1; j < entries_.size(); ++j)
        {
            const auto& entry2 = entries_[j];

            if (entry2.minX > entry1.maxX)
                break;

            // Intruding disc-intruding disc: No overlap should occur here because intruders come from the parent
            // compartment and collisions are handled from parent to child
            // Membrane-intruding disc, intruding disc-membrane: Again, handled by parent
            // Membrane-membrane: Overlapping membranes aren't allowed to occur

            const bool neitherEntryIsDisc = entry1.type != EntryType::Disc && entry2.type != EntryType::Disc;
            const bool alreadyInCollision = isInCollision(entry2);
            const bool noOverlap =
                !mathutils::circlesOverlap(entry1.position, entry1.radius, entry2.position, entry2.radius);

            if (neitherEntryIsDisc || alreadyInCollision || noOverlap)
                continue;

            if (entry1.type == EntryType::Membrane || entry2.type == EntryType::Membrane)
            {
                const bool firstIsMembrane = (entry1.type == EntryType::Membrane);
                auto disc = firstIsMembrane ? &(*discs)[entry2.index] : &(*discs)[entry1.index];
                auto membrane = firstIsMembrane ? &(*membranes)[entry1.index] : &(*membranes)[entry2.index];

                collisions.discMembraneCollisions.emplace_back(disc, membrane);
                continue;
            }
            else
            {
                const bool firstIsIntruder = (entry1.type == EntryType::IntrudingDisc);
                const bool secondIsIntruder = (entry2.type == EntryType::IntrudingDisc);

                auto d1 = firstIsIntruder ? (*intrudingDiscs)[entry1.index] : &(*discs)[entry1.index];
                auto d2 = secondIsIntruder ? (*intrudingDiscs)[entry2.index] : &(*discs)[entry2.index];

                collisions.discDiscCollisions.emplace_back(d1, d2);
                ++collisionCounts_[d1->getTypeID()];
                ++collisionCounts_[d2->getTypeID()];

                if (firstIsIntruder)
                    intrudingDiscsInCollisions[entry1.index] = 1;
                else
                    discsInCollisions[entry1.index] = 1;

                if (secondIsIntruder)
                    intrudingDiscsInCollisions[entry2.index] = 1;
                else
                    discsInCollisions[entry2.index] = 1;

                // Multiple collisions for discs aren't supported, so stop searching further
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