#include "CollisionHandler.hpp"
#include "Disc.hpp"
#include "MathUtils.hpp"

namespace cell
{

CollisionHandler::CollisionHandler(DiscTypeResolver discTypeResolver)
    : discTypeResolver_(std::move(discTypeResolver))
{
}

void CollisionHandler::calculateDiscDiscCollisionResponse(std::set<std::pair<Disc*, Disc*>>& discDiscCollisions) const
{
    for (const auto& [p1, p2] : discDiscCollisions)
    {
        if (p1->isMarkedDestroyed() || p2->isMarkedDestroyed())
            continue;

        // TODO The disc type resolver calls could be avoided here by caching the disc types as members

        const auto& overlap = calculateOverlap(*p1, *p2);

        // No overlap -> no collision
        if (overlap <= 0)
            continue;

        double dt = calculateTimeBeforeCollision(*p1, *p2);

        p1->move(dt * p1->getVelocity());
        p2->move(dt * p2->getVelocity());

        updateVelocitiesAtCollision(*p1, *p2);

        p1->move(-dt * p1->getVelocity());
        p2->move(-dt * p2->getVelocity());
    }
}

void CollisionHandler::calculateDiscRectangleCollisionResponse(
    Disc& disc, CollisionDetector::RectangleCollision& rectangleCollision) const
{
    if (!rectangleCollision.isCollision())
        return;

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
}

double CollisionHandler::keepKineticEnergyConstant(Disc& disc, const CollisionDetector::RectangleCollision& collision,
                                                   double deficiency) const
{
    if (!collision.isCollision() || deficiency <= 0)
        return 0.0;

    // The constant has to be selected so that enough energy gets transferred to the disc to even out the deficiency but
    // not too much to make it look stupid
    auto randomNumber = mathutils::getRandomNumber<double>(0, 0.15);
    double kineticEnergyBefore = disc.getKineticEnergy(discTypeResolver_);
    disc.scaleVelocity(1.0 + randomNumber);

    return disc.getKineticEnergy(discTypeResolver_) - kineticEnergyBefore;
}

double CollisionHandler::calculateOverlap(const Disc& d1, const Disc& d2) const
{
    sf::Vector2d rVec = d2.getPosition() - d1.getPosition();
    double distance = mathutils::abs(rVec);

    return discTypeResolver_(d1.getDiscTypeID()).getRadius() + discTypeResolver_(d2.getDiscTypeID()).getRadius() -
           distance;
}

double CollisionHandler::calculateTimeBeforeCollision(const Disc& d1, const Disc& d2) const
{
    const auto& r = d2.getPosition() - d1.getPosition();
    sf::Vector2d v = d2.getVelocity() - d1.getVelocity();

    const auto& r1 = discTypeResolver_(d1.getDiscTypeID()).getRadius();
    const auto& r2 = discTypeResolver_(d2.getDiscTypeID()).getRadius();

    return (-r.x * v.x - r.y * v.y -
            std::sqrt(-r.x * r.x * v.y * v.y + 2 * r.x * r.y * v.x * v.y - r.y * r.y * v.x * v.x +
                      ((r1 + r2) * (r1 + r2)) * (v.x * v.x + v.y * v.y))) /
           (v.x * v.x + v.y * v.y);
}

void CollisionHandler::updateVelocitiesAtCollision(Disc& d1, Disc& d2) const
{
    static const double e = 1.;

    // Note that discs are moved to right when they were touching before calling this function so the distance will
    // never be 0 between them

    sf::Vector2d rVec = d2.getPosition() - d1.getPosition();
    sf::Vector2d nVec = rVec / mathutils::abs(rVec);

    double vrN = (d1.getVelocity() - d2.getVelocity()) * nVec;
    const auto& m1 = discTypeResolver_(d1.getDiscTypeID()).getMass();
    const auto& m2 = discTypeResolver_(d2.getDiscTypeID()).getMass();

    double impulse = -vrN * (e + 1) / (1. / m1 + 1. / m2);

    d1.accelerate(impulse / m1 * nVec);
    d2.accelerate(-impulse / m2 * nVec);
}

} // namespace cell