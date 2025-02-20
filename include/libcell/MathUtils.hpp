#ifndef MATHUTILS_HPP
#define MATHUTILS_HPP

#include "Disc.hpp"

#include <SFML/System/Time.hpp>

#include <set>
#include <vector>

namespace MathUtils
{

std::vector<Disc> decomposeDiscs(std::vector<Disc>& discs);
std::set<std::pair<Disc*, Disc*>> findCollidingDiscs(std::vector<Disc>& discs, int maxRadius);
int handleDiscCollisions(const std::set<std::pair<Disc*, Disc*>>& collidingDiscs);
void handleWorldBoundCollision(Disc& disc, const sf::Vector2f& bounds);

} // namespace MathUtils

#endif /* MATHUTILS_HPP */
