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

        if (collision.type == CollisionType::DiscContainingMembrane)
        {
            const auto& disc = (*params.discs)[collision.i];
            const auto& membrane = *params.containingMembrane;

            sf::Vector2d diff = disc.getPosition() - membrane.getPosition();
            const auto distance = mathutils::abs(diff);
            normals.push_back(diff / distance);

            const auto R1 = discTypeRegistry_.getByID(disc.getTypeID()).getRadius();
            const auto R2 = membraneTypeRegistry_.getByID(membrane.getTypeID()).getRadius();

            betaC.push_back(beta * (R1 - R2 - distance));
        }
        else
        {
            bool isMembraneCollision = collision.type == CollisionType::DiscChildMembrane;
            const auto r1 = (*params.discs)[collision.i].getPosition();
            const auto r2 = isMembraneCollision ? (*params.membranes)[collision.j].getPosition()
                                                : (*params.discs)[collision.j].getPosition();

            const auto R1 = discTypeRegistry_.getByID((*params.discs)[collision.i].getTypeID()).getRadius();
            double R2;
            if (isMembraneCollision)
                R2 = membraneTypeRegistry_.getByID((*params.membranes)[collision.j].getTypeID()).getRadius();
            else
                R2 = discTypeRegistry_.getByID((*params.discs)[collision.j].getTypeID()).getRadius();

            const sf::Vector2d diff = r2 - r1;
            const auto distance = mathutils::abs(diff);
            normals.push_back(diff / distance);

            betaC.push_back(beta * (distance - (R1 + R2)));
        }
    }
}

} // namespace cell