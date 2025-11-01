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

CollisionDetector::Collisions CollisionDetector::detectCollisions(const Params& params)
{
    params_ = params;
    Collisions collisions;
    collisions.discDiscCollisions.reserve(params_.discs->size() / 2);

    std::vector<char> discsInCollisions(params_.discs->size(), 0);
    std::vector<char> intrudingDiscsInCollisions(params_.intrudingDiscs ? params_.intrudingDiscs->size() : 0, 0);

    const auto markAsColliding = [&](const Entry& entry)
    {
        if (entry.type == EntryType::IntrudingDisc)
            intrudingDiscsInCollisions[entry.index] = 1;
        else
            discsInCollisions[entry.index] = 1;
    };

    const auto isInCollision = [&](const Entry& entry)
    {
        return (entry.type == EntryType::Disc && discsInCollisions[entry.index]) ||
               (entry.type == EntryType::IntrudingDisc && intrudingDiscsInCollisions[entry.index]);
    };

    const auto getDiscPointer = [&](const Entry& entry)
    {
        if (entry.type == EntryType::IntrudingDisc)
            return (*params_.intrudingDiscs)[entry.index];

        return &(*params_.discs)[entry.index];
    };

    double timeOfImpact = std::numeric_limits<double>::max();
    const Entry* firstEntry = nullptr;
    const Entry* otherEntry = nullptr;

    const auto updateTimeOfImpact = [&](double newTimeOfImpact, const Entry* entry1, const Entry* entry2)
    {
        if (newTimeOfImpact < timeOfImpact)
        {
            timeOfImpact = newTimeOfImpact;
            firstEntry = entry1;
            otherEntry = entry2;
        }
    };

    for (std::size_t i = 0; i < entries_.size(); ++i)
    {
        const auto& entry1 = entries_[i];
        if (isInCollision(entry1))
            continue;

        if (entry1.type == EntryType::Disc && !discIsContainedByMembrane(entry1))
        {
            double newTimeOfImpact = calculateTimeOfImpactWithContainingMembrane(entry1, *params.containingMembrane);
            updateTimeOfImpact(newTimeOfImpact, &entry1, nullptr);
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

            if ((entry1.type != EntryType::Disc && entry2.type != EntryType::Disc) || isInCollision(entry2) ||
                !mathutils::circlesOverlap(entry1.position, entry1.radius, entry2.position, entry2.radius))
                continue;

            if (entry1.type == EntryType::Membrane || entry2.type == EntryType::Membrane)
            {
                double newTimeOfImpact = calculateTimeOfImpactWithChildMembrane(entry1, entry2);
                updateTimeOfImpact(newTimeOfImpact, &entry1, &entry2);
            }
            else
            {
                double newTimeOfImpact = calculateTimeOfImpactBetweenDiscs(entry1, entry2);
                updateTimeOfImpact(newTimeOfImpact, &entry1, &entry2);
            }
        }

        if (otherEntry)
        {
            markAsColliding(*otherEntry);

            if (otherEntry->type == EntryType::Disc)
                collisions.discDiscCollisions.emplace_back(&(*params_.discs)[entry1.index],
                                                           getDiscPointer(*otherEntry));
            else
                collisions.discChildMembraneCollisions.emplace_back(&(*params_.discs)[entry1.index],
                                                                    &(*params_.membranes)[otherEntry->index]);
        }
        else if (firstEntry)
            collisions.discContainingMembraneCollisions.push_back(&(*params_.discs)[entry1.index]);

        firstEntry = nullptr;
        otherEntry = nullptr;
        timeOfImpact = std::numeric_limits<double>::max();
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

double CollisionDetector::calculateTimeOfImpactWithContainingMembrane(const Entry& entry,
                                                                      const Membrane& containingMembrane) const
{
    const auto& disc = (*params_.discs)[entry.index];
    const auto& r = disc.getPosition();
    const auto& v = disc.getVelocity();
    const auto Rd = discTypeRegistry_.getByID(disc.getTypeID()).getRadius();
    const auto Rm = membraneTypeRegistry_.getByID(containingMembrane.getTypeID()).getRadius();
    const auto M = containingMembrane.getPosition();

    const double p = -2.0 * (M - r) * v / (v * v);
    const double q = ((M - r) * (M - r) - (Rm - Rd) * (Rm - Rd)) / (v * v);

    const double dt = 0.5 * (-p + std::sqrt(p * p - 4 * q));

    return dt;
}

double CollisionDetector::calculateTimeOfImpactWithChildMembrane(const Entry& entry1, const Entry& entry2) const
{
    const bool firstIsMembrane = (entry1.type == EntryType::Membrane);
    const auto& disc = firstIsMembrane ? (*params_.discs)[entry2.index] : (*params_.discs)[entry1.index];
    const auto& membrane = firstIsMembrane ? (*params_.membranes)[entry1.index] : (*params_.membranes)[entry2.index];

    const auto r = disc.getPosition() - membrane.getPosition();
    const auto v = disc.getVelocity();

    const double dt =
        mathutils::calculateTimeBeforeCollision(r, v, membraneTypeRegistry_.getByID(membrane.getTypeID()).getRadius(),
                                                discTypeRegistry_.getByID(disc.getTypeID()).getRadius());

    return dt;
}

double CollisionDetector::calculateTimeOfImpactBetweenDiscs(const Entry& entry1, const Entry& entry2) const
{
    const bool firstIsIntruder = (entry1.type == EntryType::IntrudingDisc);
    const bool secondIsIntruder = (entry2.type == EntryType::IntrudingDisc);

    const auto& d1 = firstIsIntruder ? *(*params_.intrudingDiscs)[entry1.index] : (*params_.discs)[entry1.index];
    const auto& d2 = secondIsIntruder ? *(*params_.intrudingDiscs)[entry2.index] : (*params_.discs)[entry2.index];

    const auto r = d2.getPosition() - d1.getPosition();
    const auto v = d2.getVelocity() - d1.getVelocity();

    const double dt =
        mathutils::calculateTimeBeforeCollision(r, v, discTypeRegistry_.getByID(d1.getTypeID()).getRadius(),
                                                discTypeRegistry_.getByID(d2.getTypeID()).getRadius());

    return dt;
}

} // namespace cell