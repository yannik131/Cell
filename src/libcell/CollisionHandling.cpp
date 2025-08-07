#include "CollisionHandling.hpp"
#include "Disc.hpp"
#include "GlobalSettings.hpp"
#include "MathUtils.hpp"
#include "Membrane.hpp"
#include "Vector2d.hpp"

namespace cell::collisions
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
 * @returns Instance of `OverlapResults` with appropriate values if the positions of the 2 circles aren't identical,
 * otherwise every entry of the return `OverlapResults` is 0.
 */
OverlapResults calculateOverlap(const sf::Vector2d& p1, double r1, const sf::Vector2d& p2, double r2)
{
    sf::Vector2d rVec = p2 - p1;
    double distance = mathutils::abs(rVec);

    if (distance == 0)
        return OverlapResults{.rVec = {0, 0}, .nVec = {0, 0}, .distance = 0, .overlap = 0};

    sf::Vector2d nVec = rVec / distance;
    double overlap = r1 + r2 - distance;

    return OverlapResults{.rVec = rVec, .nVec = nVec, .distance = distance, .overlap = overlap};
}

/**
 * @brief Given 2 circles, returns the earlier of the 2 times where they just started touching
 * @note If the overlap was calculated with `calculateOverlap(disc1Position, disc1Radius, disc2Position, disc2Radius)`
 * then the same order of the discs needs to be used for this function as well, i. e.
 * `calculateTimeBeforeCollision(disc1Velocity, disc1Radius, disc2Velocity, disc2Radius, overlapResults)`. Otherwise the
 * sign of returned dt will be incorrect.
 */
double calculateTimeBeforeCollision(const sf::Vector2d& v1, double r1, const sf::Vector2d& v2, double r2,
                                    const OverlapResults& overlapResults)
{
    const auto& r = overlapResults.rVec;
    sf::Vector2d v = v2 - v1;

    return (-r.x * v.x - r.y * v.y -
            std::sqrt(-r.x * r.x * v.y * v.y + 2 * r.x * r.y * v.x * v.y - r.y * r.y * v.x * v.x +
                      ((r1 + r2) * (r1 + r2)) * (v.x * v.x + v.y * v.y))) /
           (v.x * v.x + v.y * v.y);
}

/**
 * @brief Given 2 colliding discs, calculates their new velocities based on a classical collision response
 */
void updateVelocitiesAtCollision(Disc& d1, Disc& d2)
{
    static const double e = 1.;

    sf::Vector2d rVec = d2.getPosition() - d1.getPosition();
    sf::Vector2d nVec = rVec / mathutils::abs(rVec);

    double vrN = (d1.getVelocity() - d2.getVelocity()) * nVec;
    const auto& m1 = d1.getType()->getMass();
    const auto& m2 = d2.getType()->getMass();

    double impulse = -vrN * (e + 1) / (1. / m1 + 1. / m2);

    d1.accelerate(impulse / m1 * nVec);
    d2.accelerate(-impulse / m2 * nVec);
}

} // namespace

DiscType::map<int> calculateDiscDiscCollisionResponse(std::set<std::pair<Disc*, Disc*>>& discDiscCollisions)
{
    DiscType::map<int> collisionCounts;

    for (const auto& [p1, p2] : discDiscCollisions)
    {
        const OverlapResults& overlapResults = calculateOverlap(p1->getPosition(), p1->getType()->getRadius(),
                                                                p2->getPosition(), p2->getType()->getRadius());

        // No overlap -> no collision
        if (overlapResults.overlap <= 0)
            continue;

        ++collisionCounts[p1->getType()];
        ++collisionCounts[p2->getType()];

        double dt = calculateTimeBeforeCollision(p1->getVelocity(), p1->getType()->getRadius(), p2->getVelocity(),
                                                 p2->getType()->getRadius(), overlapResults);

        p1->move(dt * p1->getVelocity());
        p2->move(dt * p2->getVelocity());

        updateVelocitiesAtCollision(*p1, *p2);

        p1->move(-dt * p1->getVelocity());
        p2->move(-dt * p2->getVelocity());
    }

    return collisionCounts;
}

void calculateDiscMembraneCollisionResponse(std::set<std::pair<Disc*, Membrane*>> discMembraneCollisions)
{
    const auto& dt = GlobalSettings::getSettings().simulationTimeStep_.asSeconds();

    for (auto& [disc, membrane] : discMembraneCollisions)
    {
        const auto& permeability = membrane->getType()->getPermeability(*disc->getType());
        if (permeability == MembraneType::PermeableBidirectional)
            continue;

        const auto& d = disc->getPosition() - membrane->getPosition();
        const auto& v = disc->getVelocity();

        // See disc-membrane collisions section in physics docs for details

        bool movingTowardMembrane = v * d > 0;

        if (movingTowardMembrane && permeability == MembraneType::PermeableInward ||
            !movingTowardMembrane && permeability == MembraneType::PermeableOutward)
            continue;

        const auto& overlapResults = calculateOverlap(disc->getPosition(), disc->getType()->getRadius(),
                                                      membrane->getPosition(), membrane->getType()->getRadius());

        if (overlapResults.overlap <= 0)
            continue;

        double dt =
            calculateTimeBeforeCollision(disc->getVelocity(), disc->getType()->getRadius(), membrane->getVelocity(),
                                         membrane->getType()->getRadius(), overlapResults);

        disc->move(dt * v);
        disc->setVelocity(v - 2 * (v * d) / (d * d) * d);
        disc->move(-dt * disc->getVelocity());
    }
}

void calculateDiscRectangleCollisionResponse(Disc& disc, CollisionDetector::RectangleCollision& rectangleCollision)
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

} // namespace cell::collisionhandling