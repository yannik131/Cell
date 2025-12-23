#ifndef F49356D0_94E7_4146_8837_E2FA0C87BEBB_HPP
#define F49356D0_94E7_4146_8837_E2FA0C87BEBB_HPP

/**
 * @brief The math utilities here are partly explained in the physics part of the documentation
 */

#include "Types.hpp"
#include "Vector2d.hpp"

#include <ostream>
#include <random>
#include <unordered_map>
#include <utility>

namespace cell
{
class Disc;
}

namespace cell::mathutils
{

/**
 * @param M1 Center point of contained circle
 * @param R1 Radius of contained circle
 * @param M2 Center point of containing circle
 * @param R2 Radius of containing circle
 */
inline bool circleIsFullyContainedByCircle(const Vector2d& M1, double R1, const Vector2d& M2, double R2) noexcept
{
    const auto diff = M1 - M2;

    return diff.x * diff.x + diff.y * diff.y < (R2 - R1) * (R2 - R1);
}

inline bool circlesOverlap(const Vector2d& M1, double R1, const Vector2d& M2, double R2, MinOverlap minOverlap) noexcept
{
    const auto diff = M1 - M2;
    const double R = R1 + R2 - minOverlap.value;

    return diff.x * diff.x + diff.y * diff.y <= R * R;
}

inline bool circlesOverlap(const Vector2d& M1, double R1, const Vector2d& M2, double R2) noexcept
{
    return circlesOverlap(M1, R1, M2, R2, MinOverlap{0.0});
}

inline bool circlesIntersect(const Vector2d& M1, double R1, const Vector2d& M2, double R2) noexcept
{
    // equivalent to: return circlesOverlap(...) && !circleIsFullyContainedByCircle(...)
    const auto diff = M1 - M2;
    const auto distanceSquared = diff.x * diff.x + diff.y * diff.y;

    return (distanceSquared <= (R1 + R2) * (R1 + R2)) && distanceSquared >= (R2 - R1) * (R2 - R1);
}

/**
 * @returns |vec|
 */
inline double abs(const Vector2d& vec) noexcept
{
    return std::hypot(vec.x, vec.y);
}

inline double calculateOverlap(const Vector2d& r, double R1, double R2) noexcept
{
    double distance = mathutils::abs(r);

    return R1 + R2 - distance;
}

/**
 * @brief Returns a number in the given range
 */
template <typename T> T getRandomNumber(std::type_identity_t<T> low, std::type_identity_t<T> high) noexcept
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

inline unsigned int getRandomInt() noexcept
{
    static thread_local std::minstd_rand rng{std::random_device{}()};

    return rng();
}

/**
 * @brief Calculates a grid of starting positions for discs based on the largest radius of all disc types in the
 * settings.
 */
std::vector<Vector2d> calculateGrid(double width, double height, double edgeLength);

} // namespace cell::mathutils

#endif /* F49356D0_94E7_4146_8837_E2FA0C87BEBB_HPP */
