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

std::ostream& operator<<(std::ostream& os, const sf::Vector2f& v)
{
    return os << "(" << v.x << ", " << v.y << ")";
}

float operator*(const sf::Vector2f& a, const sf::Vector2f& b)
{
    return a.x * b.x + a.y * b.y;
}

namespace cell::mathutils
{

DiscType::map<int> handleDiscCollisions(const std::set<std::pair<Disc*, Disc*>>& collidingDiscs)
{
    DiscType::map<int> collisionCounts;

    for (const auto& [p1, p2] : collidingDiscs)
    {
        const OverlapResults& overlapResults = calculateOverlap(*p1, *p2);

        // No overlap -> no collision
        if (overlapResults.distance <= 0)
            continue;

        ++collisionCounts[p1->getType()];
        ++collisionCounts[p2->getType()];

        // Don't handle collision if reaction occured
        // TODO Handle overlap after collision (or just ignore it and let it be handled in the next time step)
        if (combinationReaction(p1, p2))
            continue;

        float dt = calculateTimeBeforeCollision(*p1, *p2, overlapResults);

        p1->move(dt * p1->getVelocity());
        p2->move(dt * p2->getVelocity());

        updateVelocitiesAtCollision(*p1, *p2);

        p1->move(-dt * p1->getVelocity());
        p2->move(-dt * p2->getVelocity());

        exchangeReaction(p1, p2);
    }

    return collisionCounts;
}

float abs(const sf::Vector2f& vec)
{
    return std::hypot(vec.x, vec.y);
}

OverlapResults calculateOverlap(const Disc& d1, const Disc& d2)
{
    sf::Vector2f rVec = d2.getPosition() - d1.getPosition();
    float distance = abs(rVec);

    if (distance == 0)
    {
        static const OverlapResults badResult{.rVec = {0, 0}, .nVec = {0, 0}, .distance = 0, .overlap = 0};

        return badResult;
    }

    sf::Vector2f nVec = rVec / distance;
    float overlap = d1.getType().getRadius() + d2.getType().getRadius() - distance;

    return OverlapResults{.rVec = rVec, .nVec = nVec, .distance = distance, .overlap = overlap};
}

float calculateTimeBeforeCollision(const Disc& d1, const Disc& d2, const OverlapResults& overlapResults)
{
    const auto& r = overlapResults.rVec;
    sf::Vector2f v = d2.getVelocity() - d1.getVelocity();
    const auto& R1 = d1.getType().getRadius();
    const auto& R2 = d2.getType().getRadius();

    return (-r.x * v.x - r.y * v.y -
            std::sqrt(-r.x * r.x * v.y * v.y + 2 * r.x * r.y * v.x * v.y - r.y * r.y * v.x * v.x +
                      ((R1 + R2) * (R1 + R2)) * (v.x * v.x + v.y * v.y))) /
           (v.x * v.x + v.y * v.y);
}

void updateVelocitiesAtCollision(Disc& d1, Disc& d2)
{
    static const float e = 1.f;

    sf::Vector2f rVec = d2.getPosition() - d1.getPosition();
    sf::Vector2f nVec = rVec / abs(rVec);

    float vrN = (d1.getVelocity() - d2.getVelocity()) * nVec;
    const auto& m1 = d1.getType().getMass();
    const auto& m2 = d2.getType().getMass();

    float impulse = -vrN * (e + 1) / (1.f / m1 + 1.f / m2);

    d1.accelerate(impulse / m1 * nVec);
    d2.accelerate(-impulse / m2 * nVec);
}

float getRandomFloat()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> distribution(0, 1);

    return distribution(gen);
}

int calculateHash(int x, int y)
{
    int prime = 100003;
    return (x * prime) ^ y;
}

} // namespace cell::mathutils