#ifndef MATHUTILS_HPP
#define MATHUTILS_HPP

/**
 * @brief The math utilities here are partly explained in the physics part of the documentation
 */

#include "Disc.hpp"

#include <SFML/System/Time.hpp>

#include <set>
#include <unordered_map>
#include <vector>

namespace cell
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
    sf::Vector2f rVec;

    /**
     * @brief Normalized direction vector from first disc to second
     */
    sf::Vector2f nVec;

    /**
     * @brief Absolute distance between the 2 discs
     */
    float distance{};

    /**
     * @brief A positive value for the overlap means the discs are overlapping
     */
    float overlap{};
};

/**
 * @brief Entry-wise addition of 2 maps, i. e. {1: 1, 2: 3} + {1: 0, 2: 4} = {1: 1, 2: 7}
 */
template <typename T1, typename T2, typename T3>
std::unordered_map<T1, T2, T3>& operator+=(std::unordered_map<T1, T2, T3>& a, const std::unordered_map<T1, T2, T3>& b)
{
    for (const auto& [key, value] : b)
        a[key] += value;

    return a;
}

/**
 * @brief Entry-wise division of 2 maps
 */
template <typename T1, typename T2, typename T3, typename T4>
std::unordered_map<T1, T2, T3>& operator/=(std::unordered_map<T1, T2, T3>& a, const T4& b)
{
    for (const auto& [key, value] : a)
        a[key] /= b;

    return a;
}

/**
 * @brief Prints the x and y coordinates to the given stream
 */
std::ostream& operator<<(std::ostream& os, const sf::Vector2f& v);

/**
 * @brief Scalar product of 2 vectors
 */
float operator*(const sf::Vector2f& a, const sf::Vector2f& b);

namespace mathutils
{
/**
 * @brief Finds all discs in the vector that overlap and returns them as unique pairs.
 * @param discs The vector of discs
 * @param maxRadius The largest radius of all discs in the simulation. Used for the radius search in the kd tree to find
 * candidates for collision
 */
std::set<std::pair<Disc*, Disc*>> findCollidingDiscs(std::vector<Disc>& discs, float maxRadius);

/**
 * @brief Corrects overlapping discs, calculating their new positions and velocities after collision. Calls the
 * appropriate functions to handle combination and exchange reactions
 */
DiscType::map<int> handleDiscCollisions(const std::set<std::pair<Disc*, Disc*>>& collidingDiscs);

/**
 * @brief Makes the disc bounce back from the walls. Gives additional kinetic energy to the disc if
 * `kineticEnergyDeficiency > 0`
 * @returns the kinetic energy added to the disc, if any
 */
float handleWorldBoundCollision(Disc& disc, const sf::Vector2f& boundsTopLeft, const sf::Vector2f& boundsBottomRight,
                                float kineticEnergyDeficiency);

/**
 * @returns |vec|
 */
float abs(const sf::Vector2f& vec);

/**
 * @returns Instance of `OverlapResults` with appropriate values if the positions of the 2 discs aren't identical,
 * otherwise every entry of the return `OverlapResults` is 0.
 */
OverlapResults calculateOverlap(const Disc& d1, const Disc& d2);

/**
 * @brief Given 2 discs, returns the earlier of the 2 times where they just started touching
 */
float calculateTimeBeforeCollision(const Disc& d1, const Disc& d2, const OverlapResults& overlapResults);

/**
 * @brief Given 2 colliding discs, calculates their new velocities based on a classical collision response
 */
void updateVelocitiesAtCollision(Disc& d1, Disc& d2);

/**
 * @brief Returns a random float within [0, 1)
 */
float getRandomFloat();

/**
 * @return pair where pair.first <= pair.second
 */
template <typename T> std::pair<T, T> makeOrderedPair(const T& a, const T& b)
{
    if (a <= b)
        return std::make_pair(a, b);

    return std::make_pair(b, a);
}

/**
 * @returns Unique integer for the given arguments
 */
int calculateHash(int x, int y);

} // namespace mathutils

} // namespace cell

#endif /* MATHUTILS_HPP */
