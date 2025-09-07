#include "MathUtils.hpp"
#include "Disc.hpp"

#include <glog/logging.h>
#include <nanoflann.hpp>

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

namespace
{

std::random_device rd;
std::mt19937 gen(rd());

} // namespace

double abs(const sf::Vector2d& vec)
{
    return std::hypot(vec.x, vec.y);
}

std::vector<sf::Vector2d> calculateGrid(int width, int height, int edgeLength)
{
    std::vector<sf::Vector2d> gridPoints;
    gridPoints.reserve(static_cast<std::size_t>((width / edgeLength) * (height / edgeLength)));
    double spacing = edgeLength + 1;

    for (int i = 0; i < static_cast<int>(width / (2 * spacing)); ++i)
    {
        for (int j = 0; j < static_cast<int>(height / (2 * spacing)); ++j)
            gridPoints.emplace_back(spacing * static_cast<double>(2 * i + 1), spacing * static_cast<double>(2 * j + 1));
    }

    std::shuffle(gridPoints.begin(), gridPoints.end(), gen);

    return gridPoints;
}

} // namespace cell::mathutils