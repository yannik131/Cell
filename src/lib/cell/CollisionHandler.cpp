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

void CollisionHandler::resolveCollisions(const std::vector<CollisionDetector::Collision>& collisions,
                                         CollisionDetector::Params params, double dt) const
{
    if (collisions.empty())
        return;

    using CollisionType = CollisionDetector::CollisionType;
    std::vector<sf::Vector2d> normals;

    normals.reserve(collisions.size());

    struct Entry
    {
        PhysicalObject* obj;
        double invMass;
    };
    std::vector<std::pair<Entry, Entry>> collisionObjects;
    collisionObjects.reserve(collisions.size());

    const double e = 1.0;

    for (const auto& collision : collisions)
    {
        Disc* obj1 = &(*params.discs)[collision.i];
        const double invMass1 = 1.0 / discTypeRegistry_.getByID(obj1->getTypeID()).getMass();

        auto getObj2Properties = [&](const auto& c) -> Entry
        {
            switch (c.type)
            {
            case CollisionType::DiscContainingMembrane:
            {
                auto* m = params.containingMembrane;
                return Entry{.obj = m, .invMass = 0};
            }
            case CollisionType::DiscChildMembrane:
            {
                auto* m = &(*params.membranes)[c.j];
                return Entry{.obj = m, .invMass = 0};
            }
            case CollisionType::DiscIntrudingDisc:
            {
                auto* d = (*params.intrudingDiscs)[c.j];
                return Entry{.obj = d, .invMass = 1.0 / discTypeRegistry_.getByID(d->getTypeID()).getMass()};
            }
            case CollisionType::DiscDisc:
            default:
            {
                auto* d = &(*params.discs)[c.j];
                return Entry{.obj = d, .invMass = 1.0 / discTypeRegistry_.getByID(d->getTypeID()).getMass()};
            }
            }
        };

        auto entry2 = getObj2Properties(collision);
        collisionObjects.push_back({Entry{.obj = obj1, .invMass = invMass1}, entry2});

        sf::Vector2d diff = entry2.obj->getPosition() - obj1->getPosition();
        const auto distance = mathutils::abs(diff);

        if (collision.type == CollisionType::DiscChildMembrane)
            normals.push_back(-diff / distance);
        else
            normals.push_back(diff / distance);
    }

    for (std::size_t i = 0; i < collisions.size(); ++i)
    {
        double JV;
        double effMass;
        const auto& collision = collisions[i];
        const auto& [entry1, entry2] = collisionObjects[i];

        if (collision.type == CollisionType::DiscContainingMembrane)
        {
            JV = normals[i] * entry1.obj->getVelocity();
            effMass = 1.0 / entry1.invMass;
        }
        else if (collision.type == CollisionType::DiscChildMembrane)
        {
            JV = normals[i] * entry1.obj->getVelocity();
            effMass = 1.0 / entry1.invMass;
        }
        else
        {
            JV = normals[i] * (entry2.obj->getVelocity() - entry1.obj->getVelocity());
            effMass = 1.0 / (entry1.invMass + entry2.invMass);
        }

        double lambdaRaw = -effMass * (1 + e) * JV;
        double dLambda = std::max(lambdaRaw, 0.0);

        switch (collision.type)
        {
        case CollisionType::DiscContainingMembrane:
            entry1.obj->accelerate(dLambda * normals[i] * entry1.invMass);
            break;
        case CollisionType::DiscChildMembrane: entry1.obj->accelerate(dLambda * normals[i] * entry1.invMass); break;
        case CollisionType::DiscIntrudingDisc:
        case CollisionType::DiscDisc:
        default:
            entry1.obj->accelerate(-dLambda * normals[i] * entry1.invMass);
            entry2.obj->accelerate(dLambda * normals[i] * entry2.invMass);
        }
    }
}

} // namespace cell