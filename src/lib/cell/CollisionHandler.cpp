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
    std::vector<std::pair<Disc*, Disc*>>& discDiscCollisions) const
{
    for (const auto& [p1, p2] : discDiscCollisions)
        updateVelocitiesDiscDiscCollision(*p1, *p2);
}

void CollisionHandler::calculateDiscMembraneCollisionResponse(
    std::vector<std::pair<Disc*, Membrane*>>& discMembraneCollisions) const
{
    for (std::size_t i = 0; i < discMembraneCollisions.size(); ++i)
    {
        const auto& [disc, membrane] = discMembraneCollisions[i];
        const auto& membraneType = membraneTypeRegistry_.getByID(membrane->getTypeID());
        const auto& permeability = membraneType.getPermeabilityFor(disc->getTypeID());

        // TODO If angle < X degree, always collide

        if (permeability == MembraneType::Permeability::Bidirectional ||
            permeability == MembraneType::Permeability::Inward)
            continue;

        updateVelocitiesDiscMembraneCollision(*disc, *membrane);

        // The disc-membrane collisions are used later to check if a disc is intruding or moving into another membrane
        // If there was a collision response, the disc can't move into the membrane and the collision has to be removed
        std::swap(discMembraneCollisions[i], discMembraneCollisions.back());
        discMembraneCollisions.pop_back();
        --i;
    }
}

void CollisionHandler::calculateCircularBoundsCollisionResponse(Disc& disc, const sf::Vector2d& M, double Rm) const
{
    const auto& r = disc.getPosition();
    const auto& v = disc.getVelocity();
    const auto Rd = discTypeRegistry_.getByID(disc.getTypeID()).getRadius();

    const double p = -2.0 * (M - r) * v / (v * v);
    const double q = ((M - r) * (M - r) - (Rm - Rd) * (Rm - Rd)) / (v * v);

    const double dt = 0.5 * (-p + std::sqrt(p * p - 4 * q));

#ifdef DEBUG
    if (std::isnan(dt))
        throw ExceptionWithLocation("dt is nan: sqrt(x) with x < 0, probably no collision or math error");
#endif

    if (std::abs(dt) < 1e-8)
        return;

    // dt is < 0: Move the disc back in time to first point of contact
    disc.move(dt * v);
    disc.setVelocity(mathutils::reflectVector(disc.getVelocity(), disc.getPosition() - M, Rm - Rd));
    disc.move(-dt * disc.getVelocity());
}

void CollisionHandler::updateVelocitiesDiscDiscCollision(Disc& d1, Disc& d2) const
{
    const auto r = d2.getPosition() - d1.getPosition();
    const auto v = d2.getVelocity() - d1.getVelocity();
    const auto R1 = discTypeRegistry_.getByID(d1.getTypeID()).getRadius();
    const auto R2 = discTypeRegistry_.getByID(d2.getTypeID()).getRadius();

    double dt = mathutils::calculateTimeBeforeCollision(r, v, R1, R2);

    // dt is < 0: Move the disc back in time to first point of contact
    d1.move(dt * d1.getVelocity());
    d2.move(dt * d2.getVelocity());

    static const double e = 1.;

    const sf::Vector2d n = (d2.getPosition() - d1.getPosition()) / (R1 + R2);
    const double vrN = -v * n; // formula has v1 - v2

    const auto& m1 = discTypeRegistry_.getByID(d1.getTypeID()).getMass();
    const auto& m2 = discTypeRegistry_.getByID(d2.getTypeID()).getMass();

    const double impulse = -vrN * (e + 1) / (1. / m1 + 1. / m2);

    d1.accelerate(impulse / m1 * n);
    d2.accelerate(-impulse / m2 * n);

    d1.move(-dt * d1.getVelocity());
    d2.move(-dt * d2.getVelocity());
}

void CollisionHandler::updateVelocitiesDiscMembraneCollision(Disc& disc, const Membrane& membrane) const
{
    const auto r = disc.getPosition() - membrane.getPosition();
    const auto v = disc.getVelocity();
    const auto R1 = membraneTypeRegistry_.getByID(membrane.getTypeID()).getRadius();
    const auto R2 = discTypeRegistry_.getByID(disc.getTypeID()).getRadius();

    double dt = mathutils::calculateTimeBeforeCollision(r, v, R1, R2);

    disc.move(dt * disc.getVelocity());
    disc.setVelocity(
        mathutils::reflectVector(disc.getVelocity(), disc.getPosition() - membrane.getPosition(), R1 + R2));
    disc.move(-dt * disc.getVelocity());
}

} // namespace cell