#include "MathUtils.hpp"

#include <algorithm>
#include <cmath>
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

std::vector<sf::Vector2d> calculateGrid(double width, double height, double edgeLength)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::vector<sf::Vector2d> gridPoints;
    gridPoints.reserve(static_cast<std::size_t>((static_cast<double>(width) / edgeLength) *
                                                (static_cast<double>(height) / edgeLength)));
    double spacing = edgeLength + 1;

    for (int i = 0; i < static_cast<int>(width / (2 * spacing)); ++i)
    {
        for (int j = 0; j < static_cast<int>(height / (2 * spacing)); ++j)
            gridPoints.emplace_back(spacing * static_cast<double>(2 * i + 1), spacing * static_cast<double>(2 * j + 1));
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
    const auto& diff = M1 - M2;

    return diff.x * diff.x + diff.y * diff.y <= (R1 + R2) * (R1 + R2);
}

} // namespace cell::mathutils