#ifndef MATHUTILS_HPP
#define MATHUTILS_HPP

#include "Disc.hpp"

#include <SFML/System/Time.hpp>

#include <set>
#include <vector>

namespace MathUtils
{

std::set<std::pair<Disc*, Disc*>> findCollidingDiscs(std::vector<Disc>& discs, int maxRadius);
int handleDiscCollisions(const std::set<std::pair<Disc*, Disc*>>& collidingDiscs, const sf::Time& dt);
void handleWorldBoundCollision(Disc& disc, const sf::Vector2f& bounds);

} // namespace MathUtils

#endif /* MATHUTILS_HPP */
