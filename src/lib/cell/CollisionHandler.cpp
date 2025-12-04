#include "CollisionHandler.hpp"
#include "Disc.hpp"
#include "MathUtils.hpp"

#include <numbers>

namespace cell
{

CollisionHandler::CollisionHandler(const DiscTypeRegistry& discTypeRegistry,
                                   const MembraneTypeRegistry& membraneTypeRegistry)
    : discTypeRegistry_(discTypeRegistry)
    , membraneTypeRegistry_(membraneTypeRegistry)
{
}

void CollisionHandler::resolveCollisions(const std::vector<CollisionDetector::Collision>& collisions) const
{
    if (collisions.empty())
        return;

    using CollisionType = CollisionDetector::CollisionType;

    for (const auto& collision : collisions)
    {
        const auto context = calculateCollisionContext(collision);
        if (context.skipCollision)
            continue;

        if (context.impulseChange <= 0)
        {
            // Separation will always happen within 2 time steps
            double beta = context.penetration / 2;

            switch (collision.type)
            {
            case CollisionType::DiscContainingMembrane:
            case CollisionType::DiscChildMembrane: context.disc->move(beta * context.normal); break;
            default:
                context.disc->move(-beta * context.invMass1 * context.effMass * context.normal);
                context.obj2->move(beta * context.invMass2 * context.effMass * context.normal);
            }
        }
        else
        {
            switch (collision.type)
            {
            case CollisionType::DiscContainingMembrane:
            case CollisionType::DiscChildMembrane:
                context.disc->accelerate(context.impulseChange * context.normal * context.invMass1);
                break;
            default:
                context.disc->accelerate(-context.impulseChange * context.normal * context.invMass1);
                context.obj2->accelerate(context.impulseChange * context.normal * context.invMass2);
            }
        }
    }
}

CollisionHandler::CollisionContext
CollisionHandler::calculateCollisionContext(const CollisionDetector::Collision& collision) const
{
    CollisionContext context;
    using CollisionType = CollisionDetector::CollisionType;

    const bool isMembraneCollision =
        collision.type == CollisionType::DiscContainingMembrane || collision.type == CollisionType::DiscChildMembrane;

    if (collision.invalidatedByDestroyedDisc() || collision.allowedToPass)
    {
        context.skipCollision = true;
        return context;
    }

    context.disc = collision.disc;
    context.invMass1 = 1.0 / discTypeRegistry_.getByID(context.disc->getTypeID()).getMass();
    const double R1 = discTypeRegistry_.getByID(context.disc->getTypeID()).getRadius();

    double R2 = NAN;
    if (isMembraneCollision)
    {
        context.obj2 = collision.membrane;
        R2 = membraneTypeRegistry_.getByID(collision.membrane->getTypeID()).getRadius();
        context.invMass2 = 0;
    }
    else
    {
        context.obj2 = collision.otherDisc;
        context.invMass2 = 1.0 / discTypeRegistry_.getByID(collision.otherDisc->getTypeID()).getMass();
        R2 = discTypeRegistry_.getByID(collision.otherDisc->getTypeID()).getRadius();
    }

    context.effMass = 1.0 / (context.invMass1 + context.invMass2);

    const sf::Vector2d diff = context.obj2->getPosition() - context.disc->getPosition();
    const double distance = mathutils::abs(diff);

    if (collision.type == CollisionType::DiscContainingMembrane)
        context.penetration = std::abs(R2 - R1 - distance);
    else
        context.penetration = std::abs(R1 + R2 - distance);

    // The normal points in the direction of the impulse change
    if (std::abs(distance) < 1e-3)
        context.normal = sf::Vector2d{1, 0};
    else if (collision.type == CollisionType::DiscChildMembrane)
        context.normal = -diff / distance;
    else
        context.normal = diff / distance;

    double relativeNormalSpeed = NAN;
    const double e = 1.0;

    if (isMembraneCollision)
        relativeNormalSpeed = context.normal * collision.disc->getVelocity();
    else
        relativeNormalSpeed = context.normal * (context.obj2->getVelocity() - collision.disc->getVelocity());

    context.impulseChange = -context.effMass * (1 + e) * relativeNormalSpeed;

    return context;
}

} // namespace cell