#include "CollisionHandler.hpp"
#include "Disc.hpp"
#include "MathUtils.hpp"

namespace cell
{

CollisionHandler::CollisionHandler(const DiscTypeRegistry& discTypeRegistry,
                                   const MembraneTypeRegistry& membraneTypeRegistry)
    : discTypeRegistry_(discTypeRegistry)
    , membraneTypeRegistry_(membraneTypeRegistry)
{
}

void CollisionHandler::resolveCollisions(const CollisionDetector::DetectedCollisions& detectedCollisions,
                                         double dt) const
{
    const auto& collisions = detectedCollisions.collisions;
    if (collisions.empty())
        return;

    using CollisionType = CollisionDetector::CollisionType;
    struct CollisionEntry
    {
        PhysicalObject* obj1 = nullptr;
        PhysicalObject* obj2 = nullptr;
        sf::Vector2d normal;
        double penetration = 0;
        double effMass = 0;
        double invMass1 = 0;
        double invMass2 = 0;
    };
    std::vector<CollisionEntry> collisionEntries;
    collisionEntries.reserve(collisions.size());

    const auto getPermeabilityFor = [&](Membrane* membrane, Disc* disc)
    { return membraneTypeRegistry_.getByID(membrane->getTypeID()).getPermeabilityFor(disc->getTypeID()); };

    for (const auto& collision : collisions)
    {
        if (collision.isInvalidatedByDestroyedDisc())
        {
            collisionEntries.push_back({});
            continue;
        }

        CollisionEntry entry;
        entry.obj1 = collision.disc;
        const double m1 = discTypeRegistry_.getByID(static_cast<Disc*>(entry.obj1)->getTypeID()).getMass();
        entry.invMass1 = 1.0 / m1;
        const double R1 = discTypeRegistry_.getByID(static_cast<Disc*>(entry.obj1)->getTypeID()).getRadius();
        double R2;

        switch (collision.type)
        {
        case CollisionType::DiscContainingMembrane:
        case CollisionType::DiscChildMembrane:
            entry.obj2 = collision.membrane;
            R2 = membraneTypeRegistry_.getByID(static_cast<Membrane*>(entry.obj2)->getTypeID()).getRadius();
            break;
        case CollisionType::DiscIntrudingDisc:
        case CollisionType::DiscDisc:
        default:
            entry.obj2 = collision.otherDisc;
            entry.invMass2 = 1.0 / discTypeRegistry_.getByID(static_cast<Disc*>(entry.obj2)->getTypeID()).getMass();
            R2 = discTypeRegistry_.getByID(static_cast<Disc*>(entry.obj2)->getTypeID()).getRadius();
        }

        entry.effMass = 1.0 / (entry.invMass1 + entry.invMass2);

        sf::Vector2d diff = entry.obj2->getPosition() - entry.obj1->getPosition();
        const double distance = mathutils::abs(diff);

        if (collision.type == CollisionType::DiscContainingMembrane)
            entry.penetration = std::abs(R2 - R1 - distance);
        else
            entry.penetration = std::abs(R1 + R2 - distance);

        if (std::abs(distance) < 1e-3)
            entry.normal = sf::Vector2d{1, 0};
        else if (collision.type == CollisionType::DiscChildMembrane)
            entry.normal = -diff / distance;
        else
            entry.normal = diff / distance;

        collisionEntries.push_back(std::move(entry));
    }

    for (std::size_t i = 0; i < collisions.size(); ++i)
    {
        const auto& collision = collisions[i];
        if (collision.isInvalidatedByDestroyedDisc())
            continue;

        double JV;
        auto& entry = collisionEntries[i];
        const double e = 1.0;

        if (collision.type == CollisionType::DiscContainingMembrane ||
            collision.type == CollisionType::DiscChildMembrane)
        {
            const auto permeability = getPermeabilityFor(collision.membrane, collision.disc);
            if (permeability == MembraneType::Permeability::Bidirectional ||
                (collision.type == CollisionType::DiscChildMembrane &&
                 permeability == MembraneType::Permeability::Inward) ||
                (collision.type == CollisionType::DiscContainingMembrane &&
                 permeability == MembraneType::Permeability::Outward))
                continue;

            JV = entry.normal * entry.obj1->getVelocity();
        }
        else
            JV = entry.normal * (entry.obj2->getVelocity() - entry.obj1->getVelocity());

        double lambdaRaw = -entry.effMass * (1 + e) * JV;
        double dLambda = std::max(lambdaRaw, 0.0);

        if (dLambda == 0)
        {
            // Separation will always happen within 2 time steps
            double beta = entry.penetration / 2;

            switch (collision.type)
            {
            case CollisionType::DiscContainingMembrane: entry.obj1->move(beta * entry.normal); break;
            case CollisionType::DiscChildMembrane: entry.obj1->move(beta * entry.normal); break;
            default:
                entry.obj1->move(-beta * entry.invMass1 * entry.effMass * entry.normal);
                entry.obj2->move(beta * entry.invMass2 * entry.effMass * entry.normal);
            }
        }
        else
        {
            switch (collision.type)
            {
            case CollisionType::DiscContainingMembrane:
                entry.obj1->accelerate(dLambda * entry.normal * entry.invMass1);
                break;
            case CollisionType::DiscChildMembrane:
                entry.obj1->accelerate(dLambda * entry.normal * entry.invMass1);
                break;
            case CollisionType::DiscIntrudingDisc:
            case CollisionType::DiscDisc:
            default:
                entry.obj1->accelerate(-dLambda * entry.normal * entry.invMass1);
                entry.obj2->accelerate(dLambda * entry.normal * entry.invMass2);
            }
        }
    }
}

} // namespace cell