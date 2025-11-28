#ifndef F49356D0_94E7_4146_8837_E2FA0C87BEBB_HPP
#define F49356D0_94E7_4146_8837_E2FA0C87BEBB_HPP

/**
 * @brief The math utilities here are partly explained in the physics part of the documentation
 */

#include "Types.hpp"
#include "Vector2d.hpp"

#include <SFML/System/Time.hpp>

#include <ostream>
#include <random>
#include <unordered_map>
#include <utility>

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

sf::Time operator*(const sf::Time& time, double factor);

/**
 * @brief Prints the x and y coordinates to the given stream
 */
std::ostream& operator<<(std::ostream& os, const sf::Vector2d& v);

/**
 * @brief Scalar product of 2 vectors
 */
double operator*(const sf::Vector2d& a, const sf::Vector2d& b);

namespace cell
{
class Disc;
}

namespace cell::mathutils
{

/**
 * @returns |vec|
 */
double abs(const sf::Vector2d& vec);

sf::Vector2d calculateNormal(const sf::Vector2d& v1, const sf::Vector2d& v2);

/**
 * @brief Returns a number in the given range
 */
template <typename T> T getRandomNumber(std::type_identity_t<T> low, std::type_identity_t<T> high)
{
    static thread_local std::mt19937 gen{std::random_device{}()};
    if constexpr (std::is_integral_v<T>)
    {
        std::uniform_int_distribution<T> dist(low, high);
        return dist(gen);
    }
    else
    {
        std::uniform_real_distribution<T> dist(low, high);
        return dist(gen);
    }
}

/**
 * @brief Calculates a grid of starting positions for discs based on the largest radius of all disc types in the
 * settings.
 */
std::vector<sf::Vector2d> calculateGrid(double width, double height, double edgeLength);

bool pointIsInCircle(const sf::Vector2d& point, const sf::Vector2d& M, double R);

/**
 * @param M1 Center point of contained circle
 * @param R1 Radius of contained circle
 * @param M2 Center point of containing circle
 * @param R2 Radius of containing circle
 */
bool circleIsFullyContainedByCircle(const sf::Vector2d& M1, double R1, const sf::Vector2d& M2, double R2);

bool circlesOverlap(const sf::Vector2d& M1, double R1, const sf::Vector2d& M2, double R2);
bool circlesOverlap(const sf::Vector2d& M1, double R1, const sf::Vector2d& M2, double R2, MinOverlap minOverlap);

bool circlesIntersect(const sf::Vector2d& M1, double R1, const sf::Vector2d& M2, double R2);

/**
 * @return `true` if the moving object at position `pos1` with velocity `velocity` is moving towards the point `point`
 */
bool isMovingTowards(const sf::Vector2d& pos1, const sf::Vector2d& velocity, const sf::Vector2d& point);

double calculateOverlap(const sf::Vector2d& r, double R1, double R2);

double getAngleBetween(const sf::Vector2d& a, const sf::Vector2d& b);

} // namespace cell::mathutils

#endif /* F49356D0_94E7_4146_8837_E2FA0C87BEBB_HPP */
