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
                                         CollisionDetector::Params params, double dt)
{
    using CollisionType = CollisionDetector::CollisionType;
    std::vector<sf::Vector2d> normals;
    std::vector<double> betaC;
    const double beta = 1 / dt * 0.1;

    normals.reserve(collisions.size());
    betaC.reserve(collisions.size());

    for (const auto& collision : collisions)
    {
        Disc* obj1 = &(*params.discs)[collision.i];
        const double R1 = discTypeRegistry_.getByID(obj1->getTypeID()).getRadius();

        auto getObj2AndR = [&](const auto& c) -> std::pair<PhysicalObject*, double>
        {
            switch (c.type)
            {
            case CollisionType::DiscContainingMembrane:
            {
                auto* m = params.containingMembrane;
                return {m, membraneTypeRegistry_.getByID(m->getTypeID()).getRadius()};
            }
            case CollisionType::DiscChildMembrane:
            {
                auto* m = &(*params.membranes)[c.j];
                return {m, membraneTypeRegistry_.getByID(m->getTypeID()).getRadius()};
            }
            case CollisionType::DiscIntrudingDisc:
            {
                auto* d = (*params.intrudingDiscs)[c.j];
                return {d, discTypeRegistry_.getByID(d->getTypeID()).getRadius()};
            }
            case CollisionType::DiscDisc:
            default:
            {
                auto* d = &(*params.discs)[c.j];
                return {d, discTypeRegistry_.getByID(d->getTypeID()).getRadius()};
            }
            }
        };

        auto [obj2, R2] = getObj2AndR(collision);

        sf::Vector2d diff = obj2->getPosition() - obj1->getPosition();
        const auto distance = mathutils::abs(diff);
        normals.push_back(diff / distance);

        const double sign = (collision.type == CollisionType::DiscContainingMembrane) ? -1.0 : +1.0;

        betaC.push_back(beta * (sign * distance - R1 - sign * R2));
    }
}

} // namespace cell