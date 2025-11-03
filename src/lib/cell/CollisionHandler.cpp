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

void CollisionHandler::calculateDiscDiscCollisionResponse(
    std::vector<CollisionDetector::DiscDiscCollision>& discDiscCollisions) const
{
    for (auto& collision : discDiscCollisions)
        updateVelocitiesDiscDiscCollision(collision);
}

void CollisionHandler::calculateDiscChildMembraneCollisionResponse(
    std::vector<CollisionDetector::DiscChildMembraneCollision>& discChildMembraneCollisions) const
{
    for (std::size_t i = 0; i < discChildMembraneCollisions.size(); ++i)
    {
        auto& collision = discChildMembraneCollisions[i];
        const auto& membraneType = membraneTypeRegistry_.getByID(collision.membrane->getTypeID());
        const auto& permeability = membraneType.getPermeabilityFor(collision.disc->getTypeID());

        // TODO If angle < X degree, always collide
        // TODO DRY violation with below func

        if (permeability == MembraneType::Permeability::Bidirectional ||
            permeability == MembraneType::Permeability::Inward)
            continue;

        updateVelocitiesDiscChildMembraneCollision(collision);

        // The disc-membrane collisions are used later to check if a disc is intruding or moving into another membrane
        // If there was a collision response, the disc can't move into the membrane and the collision has to be removed
        std::swap(discChildMembraneCollisions[i], discChildMembraneCollisions.back());
        discChildMembraneCollisions.pop_back();
        --i;
    }
}

void CollisionHandler::calculateDiscContainingMembraneCollisionResponse(
    std::vector<CollisionDetector::DiscContainingMembraneCollision>& discContainingMembraneCollisions) const
{
    for (std::size_t i = 0; i < discContainingMembraneCollisions.size(); ++i)
    {
        auto& collision = discContainingMembraneCollisions[i];
        const auto& membraneType = membraneTypeRegistry_.getByID(collision.membrane->getTypeID());
        const auto& permeability = membraneType.getPermeabilityFor(collision.disc->getTypeID());

        // TODO If angle < X degree, always collide

        if (permeability == MembraneType::Permeability::Bidirectional ||
            permeability == MembraneType::Permeability::Outward)
            continue;

        updateVelocitiesDiscContainingMembraneCollision(collision);

        // The disc-membrane collisions are used later to check if a disc is intruding or moving into another membrane
        // If there was a collision response, the disc can't move into the membrane and the collision has to be removed
        std::swap(discContainingMembraneCollisions[i], discContainingMembraneCollisions.back());
        discContainingMembraneCollisions.pop_back();
        --i;
    }
}

void CollisionHandler::updateVelocitiesDiscDiscCollision(CollisionDetector::DiscDiscCollision& collision) const
{
    Disc* d1 = collision.disc1;
    Disc* d2 = collision.disc2;

    const auto R1 = discTypeRegistry_.getByID(d1->getTypeID()).getRadius();
    const auto R2 = discTypeRegistry_.getByID(d2->getTypeID()).getRadius();

    // dt is < 0: Move the disc back in time to first point of contact
    collision.disc1->move(collision.toi * collision.disc1->getVelocity());
    collision.disc2->move(collision.toi * collision.disc2->getVelocity());

    static const double e = 1.;

    const sf::Vector2d n = (d2->getPosition() - d1->getPosition()) / (R1 + R2);
    const double vrN = (d1->getVelocity() - d2->getVelocity()) * n;

    const auto m1 = discTypeRegistry_.getByID(d1->getTypeID()).getMass();
    const auto m2 = discTypeRegistry_.getByID(d2->getTypeID()).getMass();

    const double impulse = -vrN * (e + 1) / (1. / m1 + 1. / m2);

    d1->accelerate(impulse / m1 * n);
    d2->accelerate(-impulse / m2 * n);

    collision.disc1->move(-collision.toi * collision.disc1->getVelocity());
    collision.disc2->move(-collision.toi * collision.disc2->getVelocity());
}

void CollisionHandler::updateVelocitiesDiscChildMembraneCollision(
    CollisionDetector::DiscChildMembraneCollision& collision) const
{
    const auto Rm = membraneTypeRegistry_.getByID(collision.membrane->getTypeID()).getRadius();
    const auto Rd = discTypeRegistry_.getByID(collision.disc->getTypeID()).getRadius();

    collision.disc->move(collision.toi * collision.disc->getVelocity());
    collision.disc->setVelocity(mathutils::reflectVector(
        collision.disc->getVelocity(), collision.disc->getPosition() - collision.membrane->getPosition(), Rm + Rd));
    collision.disc->move(-collision.toi * collision.disc->getVelocity());
}

void CollisionHandler::updateVelocitiesDiscContainingMembraneCollision(
    CollisionDetector::DiscContainingMembraneCollision& collision) const
{
    const auto Rd = discTypeRegistry_.getByID(collision.disc->getTypeID()).getRadius();
    const auto Rm = membraneTypeRegistry_.getByID(collision.membrane->getTypeID()).getRadius();

    // dt is < 0: Move the disc back in time to first point of contact
    collision.disc->move(collision.toi * collision.disc->getVelocity());
    collision.disc->setVelocity(mathutils::reflectVector(
        collision.disc->getVelocity(), collision.disc->getPosition() - collision.membrane->getPosition(), Rm - Rd));
    collision.disc->move(-collision.toi * collision.disc->getVelocity());
}

} // namespace cell