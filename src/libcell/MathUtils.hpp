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

/**
 * @brief Entry-wise addition of 2 maps, i. e. {1: 1, 2: 3} + {1: 0, 2: 4} = {1: 1, 2: 7}
 */
template <typename T1, typename T2, typename T3, typename T4>
std::unordered_map<T1, T2, T3, T4>& operator+=(std::unordered_map<T1, T2, T3, T4>& a,
                                               const std::unordered_map<T1, T2, T3, T4>& b)
{
    for (const auto& [key, value] : b)
        a[key] += value;

    return a;
}

/**
 * @brief Entry-wise division of 2 maps
 */
template <typename T1, typename T2, typename T3, typename T4, typename T5>
std::unordered_map<T1, T2, T3, T5>& operator/=(std::unordered_map<T1, T2, T3, T5>& a, const T4& b)
{
    for (const auto& [key, value] : a)
        a[key] /= b;

    return a;
}

template <typename T1, typename T2, typename T3, typename T4, typename T5>
std::unordered_map<T1, T2, T3, T5> operator*(std::unordered_map<T1, T2, T3, T5> a, const T4& b)
{
    for (const auto& [key, value] : a)
        a[key] *= b;

    return a;
}

/**
 * @brief Prints the x and y coordinates to the given stream
 */
std::ostream& operator<<(std::ostream& os, const sf::Vector2d& v);

/**
 * @brief Scalar product of 2 vectors
 */
double operator*(const sf::Vector2d& a, const sf::Vector2d& b);

namespace cell::mathutils
{

/**
 * @brief Corrects overlapping discs, calculating their new positions and velocities after collision. Calls the
 * appropriate functions to handle combination and exchange reactions
 */
DiscType::map<int> handleDiscCollisions(const std::set<std::pair<Disc*, Disc*>>& collidingDiscs);

/**
 * @returns |vec|
 */
double abs(const sf::Vector2d& vec);

double distance(const sf::Vector2d& v1, const sf::Vector2d& v2);

/**
 * @returns `true` if the circle described with `pos1` and `radius1` intersects with or contains the second circle
 * described with `pos2` and `radius2`
 */
bool contains(const sf::Vector2d& pos1, double radius1, const sf::Vector2d& pos2, double radius2);

/**
 * @brief Returns a random double within [0, 1)
 */
double getRandomFloat();

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

} // namespace cell::mathutils

#endif /* MATHUTILS_HPP */
