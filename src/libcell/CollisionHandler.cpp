#include "CollisionHandler.hpp"
#include "Disc.hpp"
#include "Vector2d.hpp"

namespace cell
{
void CollisionHandler::calculateDiscDiscCollisionResponse(std::set<std::pair<Disc*, Disc*>>& discDiscCollisions) const
{
}

void CollisionHandler::calculateDiscMembraneCollisionResponse(std::set<std::pair<Disc*, Membrane*>>) const
{
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