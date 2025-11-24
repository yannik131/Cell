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

std::vector<CollisionDetector::Collision> CollisionDetector::detectCollisions(const Params& params)
{
    params_ = params;
    std::vector<Collision> collisions;
    collisions.reserve(50);

    const auto getDiscPointer = [&](const Entry& entry)
    {
        if (entry.type == EntryType::IntrudingDisc)
            return (*params_.intrudingDiscs)[entry.index];

        return &(*params_.discs)[entry.index];
    };

    const auto getPermeabilityFor = [&](const Membrane& membrane, const Entry& discEntry)
    {
        return membraneTypeRegistry_.getByID(membrane.getTypeID())
            .getPermeabilityFor((*params_.discs)[discEntry.index].getTypeID());
    };

    for (int i = 0; i < static_cast<int>(entries_.size()); ++i)
    {
        const auto& entry1 = entries_[i];
        if (entry1.type == EntryType::Disc && !discIsContainedByMembrane(entry1))
        {
            const auto permeability = getPermeabilityFor(*params_.containingMembrane, entry1);

            if (permeability == MembraneType::Permeability::Inward || permeability == MembraneType::Permeability::None)
                collisions.push_back(Collision{.i = i, .j = -1, .type = CollisionType::DiscContainingMembrane});
        }

        for (int j = i + 1; j < static_cast<int>(entries_.size()); ++j)
        {
            const auto& entry2 = entries_[j];

            if (entry2.minX > entry1.maxX)
                break;

            // Intruding disc-intruding disc: No overlap should occur here because intruders come from the parent
            // compartment and collisions are handled from parent to child
            // Membrane-intruding disc, intruding disc-membrane: Again, handled by parent
            // Membrane-membrane: Overlapping membranes aren't allowed to occur

            if ((entry1.type != EntryType::Disc && entry2.type != EntryType::Disc) ||
                !mathutils::circlesOverlap(entry1.position, entry1.radius, entry2.position, entry2.radius))
                continue;

            if (entry1.type == EntryType::Membrane || entry2.type == EntryType::Membrane)
            {
                const bool firstIsMembrane = entry1.type == EntryType::Membrane;

                const auto& membrane =
                    firstIsMembrane ? (*params_.membranes)[entry1.index] : (*params_.membranes)[entry2.index];
                const auto permeability = getPermeabilityFor(membrane, firstIsMembrane ? entry2 : entry1);

                if (permeability == MembraneType::Permeability::Outward ||
                    permeability == MembraneType::Permeability::None)
                    collisions.push_back(Collision{.i = firstIsMembrane ? j : i,
                                                   .j = firstIsMembrane ? i : j,
                                                   .type = CollisionType::DiscChildMembrane});
            }
            else
            {
                collisions.push_back(Collision{.i = i, .j = j, .type = CollisionType::DiscDisc});
                ++collisionCounts_[getDiscPointer(entry1)->getTypeID()];
                ++collisionCounts_[getDiscPointer(entry2)->getTypeID()];
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

bool CollisionDetector::discIsContainedByMembrane(const Entry& entry)
{
    auto disc = (*params_.discs)[entry.index];

    return mathutils::circleIsFullyContainedByCircle(
        disc.getPosition(), discTypeRegistry_.getByID(disc.getTypeID()).getRadius(),
        params_.containingMembrane->getPosition(),
        membraneTypeRegistry_.getByID(params_.containingMembrane->getTypeID()).getRadius());
}

} // namespace cell