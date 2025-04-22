#ifndef MATHUTILS_HPP
#define MATHUTILS_HPP

#include "Disc.hpp"

#include <SFML/System/Time.hpp>

#include <map>
#include <set>
#include <vector>

namespace MathUtils
{

/**
 * @brief Finds all discs in the vector that overlap and returns them as unique pairs.
 * @param discs The vector of discs
 * @param maxRadius The largest radius of all discs in the simulation. Used for the radius search in the kd tree to find
 * candidates for collision
 */
std::set<std::pair<Disc*, Disc*>> findCollidingDiscs(std::vector<Disc>& discs, int maxRadius);

/**
 * @brief Corrects overlapping discs, calculating their new positions and velocities after collision. Calls the
 * appropriate functions to handle combination and exchange reactions
 */
std::map<DiscType, int> handleDiscCollisions(const std::set<std::pair<Disc*, Disc*>>& collidingDiscs);

/**
 * @brief Lets the discs in the passed vector decompose according to the schemes A -> B + C defined in the settings with
 * the corresponding probabilities (per second)
 */
std::vector<Disc> decomposeDiscs(std::vector<Disc>& discs);

/**
 * @brief Makes the disc bounce back from the walls
 * @note Gives additional kinetic energy to the disc if there is less kinetic energy than at the start of the simulation
 */
float handleWorldBoundCollision(Disc& disc, const sf::Vector2f& bounds, float kineticEnergyDeficiency);

/**
 * @returns |vec|
 */
float abs(const sf::Vector2f& vec);

/**
 * @brief Moves intersecting discs away from another (overlap > 0). Returns intermediate results
 * @returns {normal vector, distance, overlap}
 */
std::tuple<sf::Vector2f, float, float> correctOverlap(Disc& d1, Disc& d2);

/**
 * @brief Returns a random float within [0, 1)
 */
float getRandomFloat();
} // namespace MathUtils

#endif /* MATHUTILS_HPP */
