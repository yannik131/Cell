#include "CollisionDetector.hpp"
#include "Disc.hpp"
#include "MathUtils.hpp"

#include <algorithm>

namespace cell
{

DiscTypeMap<int> CollisionDetector::collisionCounts_;

CollisionDetector::CollisionDetector(const DiscTypeRegistry& discTypeRegistry,
                                     const MembraneTypeRegistry& membraneTypeRegistry)
    : discTypeRegistry_(discTypeRegistry)
    , membraneTypeRegistry_(membraneTypeRegistry)
{
}

void CollisionDetector::setParams(Params params)
{
    params_ = std::move(params);
}

void CollisionDetector::buildMembraneIndex()
{
    membraneEntries_.clear();
    const auto& membranes = *params_.membranes;
    membraneEntries_.reserve(membranes.size());

    for (std::size_t i = 0; i < membranes.size(); ++i)
        membraneEntries_.push_back(createEntry(membranes[i], membraneTypeRegistry_, i, EntryType::Membrane));

    std::sort(membraneEntries_.begin(), membraneEntries_.end(), entryComparator_);
}

void CollisionDetector::buildDiscIndex()
{
    discEntries_.clear();
    const auto& discs = *params_.discs;
    // We guess 100 intruding discs
    discEntries_.reserve(discs.size() + 100);

    for (std::size_t i = 0; i < discs.size(); ++i)
        discEntries_.push_back(createEntry(discs[i], discTypeRegistry_, i, EntryType::Disc));

    std::sort(discEntries_.begin(), discEntries_.end(), entryComparator_);
}

void CollisionDetector::addIntrudingDiscsToIndex()
{
    const auto& intrudingDiscs = *params_.intrudingDiscs;
    const auto oldSize = discEntries_.size();

    for (std::size_t i = 0; i < intrudingDiscs.size(); ++i)
        discEntries_.push_back(createEntry(*intrudingDiscs[i], discTypeRegistry_, i, EntryType::IntrudingDisc));

    auto mid = discEntries_.begin() + static_cast<ptrdiff_t>(oldSize);
    std::sort(mid, discEntries_.end(), entryComparator_);
    std::inplace_merge(discEntries_.begin(), mid, discEntries_.end(), entryComparator_);
}

std::vector<CollisionDetector::Collision> CollisionDetector::detectDiscMembraneCollisions()
{
    std::vector<Collision> collisions;
    collisions.reserve(static_cast<std::size_t>(discEntries_.size() * 0.05));

    std::size_t startJ = 0;

    for (std::size_t i = 0; i < discEntries_.size(); ++i)
    {
        const auto& entry = discEntries_[i];
        auto* disc = &(*params_.discs)[discEntries_[i].index];

        if (!discIsContainedByMembrane(entry))
            collisions.push_back(Collision{.disc = disc,
                                           .membrane = params_.containingMembrane,
                                           .type = CollisionType::DiscContainingMembrane,
                                           .allowedToPass = canGoThrough(disc, params_.containingMembrane,
                                                                         CollisionType::DiscContainingMembrane)});

        if (startJ == membraneEntries_.size())
            continue;

        while (startJ < membraneEntries_.size() && membraneEntries_[startJ].maxX < entry.minX)
            ++startJ;

        for (std::size_t j = startJ; j < membraneEntries_.size(); ++j)
        {
            if (membraneEntries_[j].minX > entry.maxX)
                break;

            if (mathutils::circlesOverlap(entry.position, entry.radius, membraneEntries_[j].position,
                                          membraneEntries_[j].radius))
            {
                auto* membrane = &(*params_.membranes)[membraneEntries_[j].index];
                collisions.push_back(
                    Collision{.disc = disc,
                              .membrane = membrane,
                              .type = CollisionType::DiscChildMembrane,
                              .allowedToPass = canGoThrough(disc, membrane, CollisionType::DiscChildMembrane)});
            }
        }
    }

    return collisions;
}

std::vector<CollisionDetector::Collision> CollisionDetector::detectDiscDiscCollisions()
{
    std::vector<Collision> collisions;
    collisions.reserve(static_cast<std::size_t>(discEntries_.size() * 0.1));

    const auto getDiscPointer = [&](const Entry& entry)
    {
        if (entry.type == EntryType::IntrudingDisc)
            return (*params_.intrudingDiscs)[entry.index];

        return &(*params_.discs)[entry.index];
    };

    for (std::size_t i = 0; i < discEntries_.size(); ++i)
    {
        const auto& entry1 = discEntries_[i];
        for (std::size_t j = i + 1; j < discEntries_.size(); ++j)
        {
            const auto& entry2 = discEntries_[j];
            if (entry2.minX > entry1.maxX)
                break;

            if (entry2.type == EntryType::IntrudingDisc && entry1.type == EntryType::IntrudingDisc)
                continue;

            if (!mathutils::circlesOverlap(entry1.position, entry1.radius, entry2.position, entry2.radius,
                                           MinOverlap{1e-2}))
                continue;

            if (entry1.type == EntryType::IntrudingDisc)
                collisions.push_back(Collision{.disc = &(*params_.discs)[entry2.index],
                                               .otherDisc = (*params_.intrudingDiscs)[entry1.index],
                                               .type = CollisionType::DiscIntrudingDisc});
            else if (entry2.type == EntryType::IntrudingDisc)
                collisions.push_back(Collision{.disc = &(*params_.discs)[entry1.index],
                                               .otherDisc = (*params_.intrudingDiscs)[entry2.index],
                                               .type = CollisionType::DiscIntrudingDisc});
            else
                collisions.push_back(Collision{.disc = &(*params_.discs)[entry1.index],
                                               .otherDisc = &(*params_.discs)[entry2.index],
                                               .type = CollisionType::DiscDisc});

            ++collisionCounts_[getDiscPointer(entry1)->getTypeID()];
            ++collisionCounts_[getDiscPointer(entry2)->getTypeID()];
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

bool CollisionDetector::canGoThrough(Disc* disc, Membrane* membrane,
                                     CollisionDetector::CollisionType collisionType) const
{
    using CollisionType = CollisionDetector::CollisionType;

    const auto permeability =
        membraneTypeRegistry_.getByID(membrane->getTypeID()).getPermeabilityFor(disc->getTypeID());

    if (permeability == MembraneType::Permeability::Bidirectional ||
        (collisionType == CollisionType::DiscChildMembrane && permeability == MembraneType::Permeability::Inward) ||
        (collisionType == CollisionType::DiscContainingMembrane && permeability == MembraneType::Permeability::Outward))
        return true;

    return false;
}

} // namespace cell