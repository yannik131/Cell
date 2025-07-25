#include "CollisionHandler.hpp"
#include "Disc.hpp"
#include "GlobalSettings.hpp"
#include "MathUtils.hpp"
#include "Membrane.hpp"
#include "Vector2d.hpp"

namespace cell
{

namespace
{

/**
 * @brief Struct for the intermediate results of the overlap calculation, since they're useful for the following
 * collision response
 */
struct OverlapResults
{
    /**
     * @brief Direction vector from the first disc to the second
     */
    sf::Vector2d rVec;

    /**
     * @brief Normalized direction vector from first disc to second
     */
    sf::Vector2d nVec;

    /**
     * @brief Absolute distance between the 2 discs
     */
    double distance = 0;

    /**
     * @brief A positive value for the overlap means the discs are overlapping
     */
    double overlap = 0;
};

/**
 * @returns Instance of `OverlapResults` with appropriate values if the positions of the 2 discs aren't identical,
 * otherwise every entry of the return `OverlapResults` is 0.
 */
OverlapResults calculateOverlap(const Disc& d1, const Disc& d2)
{
    sf::Vector2d rVec = d2.getPosition() - d1.getPosition();
    double distance = mathutils::abs(rVec);

    if (distance == 0)
        return OverlapResults{.rVec = {0, 0}, .nVec = {0, 0}, .distance = 0, .overlap = 0};

    sf::Vector2d nVec = rVec / distance;
    double overlap = d1.getType()->getRadius() + d2.getType()->getRadius() - distance;

    return OverlapResults{.rVec = rVec, .nVec = nVec, .distance = distance, .overlap = overlap};
}

/**
 * @brief Given 2 discs, returns the earlier of the 2 times where they just started touching
 */
double calculateTimeBeforeCollision(const Disc& d1, const Disc& d2, const OverlapResults& overlapResults)
{
    const auto& r = overlapResults.rVec;
    sf::Vector2d v = d2.getVelocity() - d1.getVelocity();
    const auto& R1 = d1.getType()->getRadius();
    const auto& R2 = d2.getType()->getRadius();

    return (-r.x * v.x - r.y * v.y -
            std::sqrt(-r.x * r.x * v.y * v.y + 2 * r.x * r.y * v.x * v.y - r.y * r.y * v.x * v.x +
                      ((R1 + R2) * (R1 + R2)) * (v.x * v.x + v.y * v.y))) /
           (v.x * v.x + v.y * v.y);
}

/**
 * @brief Given 2 colliding discs, calculates their new velocities based on a classical collision response
 */
void updateVelocitiesAtCollision(Disc& d1, Disc& d2)
{
    static const double e = 1.f;

    sf::Vector2d rVec = d2.getPosition() - d1.getPosition();
    sf::Vector2d nVec = rVec / mathutils::abs(rVec);

    double vrN = (d1.getVelocity() - d2.getVelocity()) * nVec;
    const auto& m1 = d1.getType()->getMass();
    const auto& m2 = d2.getType()->getMass();

    double impulse = -vrN * (e + 1) / (1.f / m1 + 1.f / m2);

    d1.accelerate(impulse / m1 * nVec);
    d2.accelerate(-impulse / m2 * nVec);
}

} // namespace

DiscType::map<int>
CollisionHandler::calculateDiscDiscCollisionResponse(std::set<std::pair<Disc*, Disc*>>& discDiscCollisions) const
{
    DiscType::map<int> collisionCounts;

    for (const auto& [p1, p2] : discDiscCollisions)
    {
        const OverlapResults& overlapResults = calculateOverlap(*p1, *p2);

        // No overlap -> no collision
        if (overlapResults.distance <= 0)
            continue;

        ++collisionCounts[p1->getType()];
        ++collisionCounts[p2->getType()];

        double dt = calculateTimeBeforeCollision(*p1, *p2, overlapResults);

        p1->move(dt * p1->getVelocity());
        p2->move(dt * p2->getVelocity());

        updateVelocitiesAtCollision(*p1, *p2);

        p1->move(-dt * p1->getVelocity());
        p2->move(-dt * p2->getVelocity());
    }

    return collisionCounts;
}

void CollisionHandler::calculateDiscMembraneCollisionResponse(
    std::set<std::pair<Disc*, Membrane*>> discMembraneCollisions) const
{
    const auto& dt = GlobalSettings::getSettings().simulationTimeStep_.asSeconds();

    for (const auto& [disc, membrane] : discMembraneCollisions)
    {
        const auto& permeability = membrane->getType()->getPermeability(*disc->getType());
        if (permeability == MembraneType::PermeableBidirectional)
            continue;

        const sf::Vector2d& positionBefore = disc->getPosition() - dt * disc->getVelocity();
        bool containedBefore = mathutils::contains(membrane->getPosition(), membrane->getType()->getRadius(),
                                                   positionBefore, disc->getType()->getRadius());
        bool containedNow = mathutils::contains(membrane->getPosition(), membrane->getType()->getRadius(),
                                                disc->getPosition(), disc->getType()->getRadius());

        if (permeability == MembraneType::PermeableInward && !containedBefore ||
            permeability == MembraneType::PermeableOutward && containedBefore)
            continue;

        // TODO collision response between circle and outside/inside of another circle
    }
}

void CollisionHandler::calculateDiscRectangleCollisionResponse(
    Disc& disc, CollisionDetector::RectangleCollision& rectangleCollision)
{
    using Wall = CollisionDetector::RectangleCollision::Wall;

    sf::Vector2d dr;

    if (rectangleCollision.xCollision_.has_value())
    {
        if (rectangleCollision.xCollision_->first == Wall::Left)
            dr.x = 2 * rectangleCollision.xCollision_->second;
        else
            dr.x = -2 * rectangleCollision.xCollision_->second;

        disc.negateXVelocity();
    }

    if (rectangleCollision.yCollision_.has_value())
    {
        if (rectangleCollision.yCollision_->first == Wall::Top)
            dr.y = 2 * rectangleCollision.yCollision_->second;
        else
            dr.y = -2 * rectangleCollision.yCollision_->second;

        disc.negateYVelocity();
    }

    disc.move(dr);

    // TODO Handle energy expletion over time somewhere, but not here
}

} // namespace cell