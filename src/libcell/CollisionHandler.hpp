#ifndef COLLISIONHANDLER_HPP
#define COLLISIONHANDLER_HPP

#include "CollisionDetector.hpp"

#include <set>

namespace cell
{

class Disc;
class Membrane;

class CollisionHandler
{
public:
    DiscType::map<int> calculateDiscDiscCollisionResponse(std::set<std::pair<Disc*, Disc*>>& discDiscCollisions) const;

    void calculateDiscMembraneCollisionResponse(std::set<std::pair<Disc*, Membrane*>> discMembraneCollisions) const;

    void calculateDiscRectangleCollisionResponse(Disc& disc, CollisionDetector::RectangleCollision& rectangleCollision);
};

} // namespace cell

#endif /* COLLISIONHANDLER_HPP */
