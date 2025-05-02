#ifndef MATHUTILS_HPP
#define MATHUTILS_HPP

#include "Disc.hpp"

#include <SFML/System/Time.hpp>

#include <map>
#include <set>
#include <vector>

template <typename T1, typename T2, typename T3>
std::map<T1, T2, T3>& operator+=(std::map<T1, T2, T3>& a, const std::map<T1, T2, T3>& b)
{
    for (const auto& [key, value] : b)
        a[key] += value;

    return a;
}

template <typename T1, typename T2, typename T3, typename T4>
std::map<T1, T2, T3>& operator/=(std::map<T1, T2, T3>& a, const T4& b)
{
    for (const auto& [key, value] : a)
        a[key] /= b;

    return a;
}

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
DiscType::map<int> handleDiscCollisions(const std::set<std::pair<Disc*, Disc*>>& collidingDiscs);

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
 * @brief Turns a map<key, value> into a map<value, key>
 */
template <template <typename, typename> class MapType, typename KeyType, typename ValueType>
MapType<ValueType, KeyType> invertMap(const MapType<KeyType, ValueType>& map)
{
    MapType<ValueType, KeyType> inverted;

    for (auto it = map.begin(); it != map.end(); ++it)
        inverted[it.value()] = it.key();

    return inverted;
}

/**
 * @brief Returns a random float within [0, 1)
 */
float getRandomFloat();
} // namespace MathUtils

#endif /* MATHUTILS_HPP */
