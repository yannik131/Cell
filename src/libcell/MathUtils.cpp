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

namespace cell
{

std::ostream& operator<<(std::ostream& os, const sf::Vector2f& v)
{
    return os << "(" << v.x << ", " << v.y << ")";
}

float operator*(const sf::Vector2f& a, const sf::Vector2f& b)
{
    return a.x * b.x + a.y * b.y;
}

namespace mathutils
{

using AdapterType = nanoflann::L2_Simple_Adaptor<float, NanoflannAdapter>;
using KDTree = nanoflann::KDTreeSingleIndexAdaptor<AdapterType, NanoflannAdapter, 2>;

std::set<std::pair<Disc*, Disc*>> findCollidingDiscs(std::vector<Disc>& discs, float maxRadius)
{
    NanoflannAdapter adapter(discs);
    KDTree kdtree(2, adapter);
    const nanoflann::SearchParameters searchParams(0, false);

    std::set<std::pair<Disc*, Disc*>> collidingDiscs;
    static std::vector<nanoflann::ResultItem<uint32_t, float>> discsInRadius;
    std::set<Disc*> discsInCollisions;

    for (auto& disc : discs)
    {
        // We do not support multiple simultaneous collisions
        if (disc.isMarkedDestroyed() || discsInCollisions.contains(&disc))
            continue;

        discsInRadius.clear();
        const float maxCollisionDistance = disc.getType().getRadius() + maxRadius;

        // This is the most time consuming part of the whole application, next to the index build in the KDTree
        // constructor
        kdtree.radiusSearch(&disc.getPosition().x, maxCollisionDistance * maxCollisionDistance, discsInRadius,
                            searchParams);

        for (const auto& result : discsInRadius)
        {
            auto& otherDisc = discs[result.first];
            if (&otherDisc == &disc || discsInCollisions.contains(&otherDisc))
                continue;

            const float radiusSum = disc.getType().getRadius() + otherDisc.getType().getRadius();

            if (result.second <= radiusSum * radiusSum)
            {
                const auto& pair = makeOrderedPair(&disc, &otherDisc);
                collidingDiscs.insert(pair);

                discsInCollisions.insert(pair.first);
                discsInCollisions.insert(pair.second);

                break;
            }
        }
    }

    return collidingDiscs;
}

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

float handleWorldBoundCollision(Disc& disc, const sf::Vector2f& boundsTopLeft, const sf::Vector2f& boundsBottomRight,
                                float kineticEnergyDeficiency)
{
    const float& R = disc.getType().getRadius();
    const sf::Vector2f& r = disc.getPosition();

    bool collided = false;
    float l = NAN; // We must satisfy clang-tidy!
    float dx = 0, dy = 0;

    if (l = R + boundsTopLeft.x - r.x; l > 0) // Left wall
    {
        dx = 2 * l;
        disc.negateXVelocity();
        collided = true;
    }
    else if (l = R - boundsBottomRight.x + r.x; l > 0) // Right wall
    {
        dx = -2 * l;
        disc.negateXVelocity();
        collided = true;
    }

    if (l = R + boundsTopLeft.y - r.y; l > 0) // Top wall
    {
        dy = 2 * l;
        disc.negateYVelocity();
        collided = true;
    }
    else if (l = R - boundsBottomRight.y + r.y; l > 0) // Bottom wall
    {
        dy = -2 * l;
        disc.negateYVelocity();
        collided = true;
    }

    if (!collided)
        return 0.f;

    disc.move({dx, dy});

    if (kineticEnergyDeficiency <= 0)
        return 0.f;

    // Combination reactions are treated as inelastic collisions, so they don't conserve total kinetic energy. To
    // simulate constant kinetic energy, we give particles a little bump when they collide with the wall if the total
    // kinetic of the system is currently lower than it was at the start of the simulation (kineticEnergyDeficiency =
    // initialKineticEnergy - currentKineticEnergy)

    // The constant has to be selected so that enough energy gets transferred to the disc to even out the deficiency but
    // not too much to make it look stupid
    float randomNumber = getRandomFloat() * 0.75f;
    float kineticEnergyBefore = disc.getKineticEnergy();
    disc.scaleVelocity(1.f + randomNumber);

    return disc.getKineticEnergy() - kineticEnergyBefore;
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

} // namespace mathutils

} // namespace cell