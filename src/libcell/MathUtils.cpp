#include "MathUtils.hpp"
#include "GlobalSettings.hpp"
#include "NanoflannAdapter.hpp"
#include "Reactions.hpp"

#include "nanoflann.hpp"
#include <glog/logging.h>

#include <cmath>
#include <numeric>
#include <ostream>
#include <random>

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

int calculateHash(int x, int y)
{
    int prime = 100003;
    return (x * prime) ^ y;
}

} // namespace cell::mathutils