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

CollisionDetector::DetectedCollisions CollisionDetector::detectCollisions(const Params& params)
{
    params_ = params;
    DetectedCollisions result;
    auto& collisions = result.collisions;
    collisions.reserve(50);

    const auto getDiscPointer = [&](const Entry& entry)
    {
        if (entry.type == EntryType::IntrudingDisc)
            return (*params_.intrudingDiscs)[entry.index];

        return &(*params_.discs)[entry.index];
    };

    for (std::size_t i = 0; i < entries_.size(); ++i)
    {
        const auto& entry1 = entries_[i];
        if (entry1.type == EntryType::Disc && !discIsContainedByMembrane(entry1))
        {
            result.indexes[CollisionType::DiscContainingMembrane].push_back(collisions.size());
            collisions.push_back(Collision{.disc = &(*params_.discs)[entry1.index],
                                           .membrane = params_.containingMembrane,
                                           .type = CollisionType::DiscContainingMembrane});
        }

        for (std::size_t j = i + 1; j < entries_.size(); ++j)
        {
            const auto& entry2 = entries_[j];

            if (entry2.minX > entry1.maxX)
                break;

            // Intruding disc-intruding disc: No overlap should occur here because intruders come from the parent
            // compartment and collisions are handled from parent to child
            // Membrane-intruding disc, intruding disc-membrane: Again, handled by parent
            // Membrane-membrane: Overlapping membranes aren't allowed to occur

            if ((entry1.type != EntryType::Disc && entry2.type != EntryType::Disc) ||
                !mathutils::circlesOverlap(entry1.position, entry1.radius, entry2.position, entry2.radius,
                                           MinOverlap{0.1}))
                continue;

            if (entry1.type == EntryType::Membrane || entry2.type == EntryType::Membrane)
            {
                const bool firstIsMembrane = entry1.type == EntryType::Membrane;

                Membrane* membrane =
                    firstIsMembrane ? &(*params_.membranes)[entry1.index] : &(*params_.membranes)[entry2.index];

                result.indexes[CollisionType::DiscChildMembrane].push_back(collisions.size());
                Disc* disc = firstIsMembrane ? &(*params_.discs)[entry2.index] : &(*params_.discs)[entry1.index];

                collisions.push_back(
                    Collision{.disc = disc, .membrane = membrane, .type = CollisionType::DiscChildMembrane});
            }
            else
            {
                if (entry1.type == EntryType::IntrudingDisc)
                {
                    result.indexes[CollisionType::DiscIntrudingDisc].push_back(collisions.size());
                    collisions.push_back(Collision{.disc = &(*params_.discs)[entry2.index],
                                                   .otherDisc = (*params_.intrudingDiscs)[entry1.index],
                                                   .type = CollisionType::DiscIntrudingDisc});
                }
                else if (entry2.type == EntryType::IntrudingDisc)
                {
                    result.indexes[CollisionType::DiscIntrudingDisc].push_back(collisions.size());
                    collisions.push_back(Collision{.disc = &(*params_.discs)[entry1.index],
                                                   .otherDisc = (*params_.intrudingDiscs)[entry2.index],
                                                   .type = CollisionType::DiscIntrudingDisc});
                }
                else
                {
                    result.indexes[CollisionType::DiscDisc].push_back(collisions.size());
                    collisions.push_back(Collision{.disc = &(*params_.discs)[entry1.index],
                                                   .otherDisc = &(*params_.discs)[entry2.index],
                                                   .type = CollisionType::DiscDisc});
                }

                ++collisionCounts_[getDiscPointer(entry1)->getTypeID()];
                ++collisionCounts_[getDiscPointer(entry2)->getTypeID()];
            }
        }
    }

    return result;
}

DiscTypeMap<int> CollisionDetector::getAndResetCollisionCounts()
{
    auto tmp = std::move(collisionCounts_);
    collisionCounts_.clear();

    return tmp;
}

bool CollisionDetector::discIsContainedByMembrane(const Entry& entry)
{
    auto disc = (*params_.discs)[entry.index];

    return mathutils::circleIsFullyContainedByCircle(
        disc.getPosition(), discTypeRegistry_.getByID(disc.getTypeID()).getRadius(),
        params_.containingMembrane->getPosition(),
        membraneTypeRegistry_.getByID(params_.containingMembrane->getTypeID()).getRadius());
}

} // namespace cell