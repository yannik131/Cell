#ifndef COLLISIONHANDLING_HPP
#define COLLISIONHANDLING_HPP

#include "CollisionDetector.hpp"

#include <set>

namespace cell
{

class Disc;
class Membrane;

namespace collisions
{

DiscType::map<int> calculateDiscDiscCollisionResponse(std::set<std::pair<Disc*, Disc*>>& discDiscCollisions);

void calculateDiscMembraneCollisionResponse(std::set<std::pair<Disc*, Membrane*>> discMembraneCollisions);

void calculateDiscRectangleCollisionResponse(Disc& disc, CollisionDetector::RectangleCollision& rectangleCollision);

} // namespace collisions

} // namespace cell

#endif /* COLLISIONHANDLING_HPP */
