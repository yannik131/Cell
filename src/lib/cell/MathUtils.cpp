#include "MathUtils.hpp"

#include <algorithm>
#include <cmath>
#include <numbers>
#include <numeric>
#include <ostream>
#include <random>

sf::Time operator*(const sf::Time& time, double factor)
{
    return time * static_cast<float>(factor);
}

std::ostream& operator<<(std::ostream& os, const sf::Vector2d& v)
{
    return os << "(" << v.x << ", " << v.y << ")";
}

double operator*(const sf::Vector2d& a, const sf::Vector2d& b)
{
    return a.x * b.x + a.y * b.y;
}

namespace cell::mathutils
{

double abs(const sf::Vector2d& vec)
{
    return std::hypot(vec.x, vec.y);
}

unsigned int getRandomInt()
{
    static thread_local std::minstd_rand rng{std::random_device{}()};

    return rng();
}

std::vector<sf::Vector2d> calculateGrid(double width, double height, double edgeLength)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::vector<sf::Vector2d> gridPoints;
    gridPoints.reserve(static_cast<std::size_t>((static_cast<double>(width) / edgeLength) *
                                                (static_cast<double>(height) / edgeLength)));
    double spacing = edgeLength + 1;

    for (int i = 0; i < static_cast<int>(width / spacing); ++i)
    {
        for (int j = 0; j < static_cast<int>(height / spacing); ++j)
            gridPoints.emplace_back(spacing * static_cast<double>(i + 1), spacing * static_cast<double>(j + 1));
    }

    std::shuffle(gridPoints.begin(), gridPoints.end(), gen);

    return gridPoints;
}

bool pointIsInCircle(const sf::Vector2d& point, const sf::Vector2d& M, double R)
{
    const auto diff = point - M;

    return diff.x * diff.x + diff.y * diff.y < R * R;
}

bool circleIsFullyContainedByCircle(const sf::Vector2d& M1, double R1, const sf::Vector2d& M2, double R2)
{
    const auto diff = M1 - M2;

    return diff.x * diff.x + diff.y * diff.y < (R2 - R1) * (R2 - R1);
}

bool circlesOverlap(const sf::Vector2d& M1, double R1, const sf::Vector2d& M2, double R2)
{
    return circlesOverlap(M1, R1, M2, R2, MinOverlap{0.0});
}

bool circlesOverlap(const sf::Vector2d& M1, double R1, const sf::Vector2d& M2, double R2, MinOverlap minOverlap)
{
    const auto diff = M1 - M2;
    const double R = R1 + R2 - minOverlap.value;

    return diff.x * diff.x + diff.y * diff.y <= R * R;
}

bool circlesIntersect(const sf::Vector2d& M1, double R1, const sf::Vector2d& M2, double R2)
{
    // equivalent to: return circlesOverlap(...) && !circleIsFullyContainedByCircle(...)
    const auto diff = M1 - M2;
    const auto distanceSquared = diff.x * diff.x + diff.y * diff.y;

    return (distanceSquared <= (R1 + R2) * (R1 + R2)) && distanceSquared >= (R2 - R1) * (R2 - R1);
}

bool isMovingTowards(const sf::Vector2d& pos1, const sf::Vector2d& velocity, const sf::Vector2d& point)
{
    const auto diff = point - pos1;

    return velocity * diff > 0;
}

double calculateOverlap(const sf::Vector2d& r, double R1, double R2)
{
    double distance = mathutils::abs(r);

    return R1 + R2 - distance;
}

double getAngleBetween(const sf::Vector2d& a, const sf::Vector2d& b)
{
    const double lenA = abs(a);
    const double lenB = abs(b);

    if (lenA == 0.0 || lenB == 0.0)
        return 0.0; // arbitrary, no direction

    double dot = a * b / (lenA * lenB);

    // Clamp to valid range for acos
    dot = std::clamp(dot, -1.0, 1.0);

    return std::acos(dot) * 180.0 / std::numbers::pi;
}

} // namespace cell::mathutils