#include "MathUtils.hpp"
#include "Disc.hpp"
#include "Reactions.hpp"

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

double abs(const sf::Vector2d& vec)
{
    return std::hypot(vec.x, vec.y);
}

double getRandomFloat()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<double> distribution(0, 1);

    return distribution(gen);
}

} // namespace cell::mathutils