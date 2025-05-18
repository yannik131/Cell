#include "MathUtils.hpp"
#include "GlobalSettings.hpp"
#include "NanoflannAdapter.hpp"

#include "nanoflann.hpp"
#include <glog/logging.h>

#include <numeric>
#include <ostream>
#include <random>

sf::Vector2f operator*(const sf::Vector2f& a, const sf::Vector2f& b)
{
    return sf::Vector2f{a.x * b.x, a.y * b.y};
}

std::ostream& operator<<(std::ostream& os, const sf::Vector2f& v)
{
    return os << "(" << v.x << ", " << v.y << ")";
}

namespace MathUtils
{

typedef nanoflann::L2_Simple_Adaptor<float, NanoflannAdapter> AdapterType;
typedef nanoflann::KDTreeSingleIndexAdaptor<AdapterType, NanoflannAdapter, 2> KDTree;

bool combinationReaction(Disc* d1, Disc* d2)
{
    const auto& combinationReactionTable = GlobalSettings::getSettings().combinationReactions_;
    auto iter = combinationReactionTable.find(std::make_pair(d2->getType(), d1->getType()));

    if (iter == combinationReactionTable.end())
        return false;

    const auto& possibleReactions = iter->second;
    float randomNumber = getRandomFloat();
    for (const auto& reaction : possibleReactions)
    {
        if (randomNumber > reaction.getProbability())
            continue;

        const auto& resultType = reaction.getProduct1();

        // For reactions of type A + B -> C, we keep the one closer in size to C and destroy the other
        if (std::abs(resultType.getRadius() - d1->getType().getRadius()) >
            std::abs(resultType.getRadius() - d2->getType().getRadius()))
            std::swap(d1, d2);

        d1->setType(resultType);
        d1->setVelocity((d1->getType().getMass() * d1->getVelocity() + d2->getType().getMass() * d2->getVelocity()) /
                        resultType.getMass());

        d2->markDestroyed();

        return true;
    }

    return false;
}

bool exchangeReaction(Disc* d1, Disc* d2)
{
    // TODO Probabilities of exchange and combination reactions should together add up to 100%
    const auto& settings = GlobalSettings::getSettings();

    const auto& exchangeReactionTable = settings.exchangeReactions_;
    auto iter = exchangeReactionTable.find(std::make_pair(d2->getType(), d1->getType()));

    if (iter == exchangeReactionTable.end())
        return false;

    const auto& possibleReactions = iter->second;
    float randomNumber = getRandomFloat();
    for (const auto& reaction : possibleReactions)
    {
        if (randomNumber > reaction.getProbability())
            continue;

        d1->scaleVelocity(std::sqrt(d1->getType().getMass() / reaction.getProduct1().getMass()));
        d1->setType(reaction.getProduct1());

        d2->scaleVelocity(std::sqrt(d2->getType().getMass() / reaction.getProduct2().getMass()));
        d2->setType(reaction.getProduct2());

        return true;
    }

    return false;
}

void decompositionReaction(Disc* d1, std::vector<Disc>& newDiscs)
{
    const auto& decompositionReactionTable = GlobalSettings::getSettings().decompositionReactions_;
    const float& simulationTimeStep = GlobalSettings::getSettings().simulationTimeStep_.asSeconds();

    const auto& iter = decompositionReactionTable.find(d1->getType());
    if (iter == decompositionReactionTable.end())
        return;

    const auto& possibleReactions = iter->second;
    float randomNumber = getRandomFloat();
    for (const auto& reaction : possibleReactions)
    {
        if (randomNumber > reaction.getProbability() * simulationTimeStep)
            continue;

        const auto& vVec = d1->getVelocity();
        const float v = abs(vVec);
        const sf::Vector2f n = vVec / v;

        // We will let the collision handling in the next time step take care of separation
        // But we can't have identical positions, so move them a little
        Disc product1(reaction.getProduct1());
        product1.setVelocity(v * sf::Vector2f{-n.y, n.x});
        product1.setPosition(d1->getPosition() + d1->getVelocity() * 1e-4f);

        Disc product2(reaction.getProduct2());
        product2.setVelocity(v * sf::Vector2f{n.y, -n.x});
        product2.setPosition(d1->getPosition() + d1->getVelocity() * 1e-4f);

        newDiscs.push_back(std::move(product1));
        newDiscs.push_back(std::move(product2));
        d1->markDestroyed();

        return;
    }

    return;
}

std::vector<Disc> decomposeDiscs(std::vector<Disc>& discs)
{
    std::vector<Disc> newDiscs;

    for (auto& disc : discs)
        decompositionReaction(&disc, newDiscs);

    return newDiscs;
}

std::set<std::pair<Disc*, Disc*>> findCollidingDiscs(std::vector<Disc>& discs, int maxRadius)
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

        for (size_t i = 0; i < discsInRadius.size(); ++i)
        {
            auto& otherDisc = discs[discsInRadius[i].first];
            if (&otherDisc == &disc || discsInCollisions.contains(&otherDisc))
                continue;

            const float radiusSum = disc.getType().getRadius() + otherDisc.getType().getRadius();

            if (discsInRadius[i].second <= radiusSum * radiusSum)
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
    const sf::Vector2f& v = disc.getVelocity();

    const float& xmin = boundsTopLeft.x;
    const float& ymin = boundsTopLeft.y;
    const float& xmax = boundsBottomRight.x;
    const float& ymax = boundsBottomRight.y;

    bool collided = false;

    if (R + xmin - r.x > 0) // Left wall
    {
        float dt = (R + xmin - r.x) / v.x;
        disc.move({2 * dt * v.x, 0});
        disc.negateXVelocity();

        collided = true;
    }
    else if (R - xmax + r.x > 0) // Right wall
    {
        float dt = -(xmax - R - r.x) / v.x;
        disc.move({-2 * dt * v.x, 0});
        disc.negateXVelocity();

        collided = true;
    }

    if (R + ymin - r.y > 0) // Top wall
    {
        float dt = (R + ymin - r.y) / v.y;
        disc.move({0, 2 * dt * v.y});
        disc.negateYVelocity();

        collided = true;
    }
    else if (R - ymax + r.y > 0) // Bottom wall
    {
        float dt = -(ymax - R - r.y) / v.y;
        disc.move({0, -2 * dt * v.y});
        disc.negateYVelocity();

        collided = true;
    }

    if (!collided || kineticEnergyDeficiency <= 0)
        return 0.f;

    // Combination reactions are treated as inelastic collisions, so they don't conserve total kinetic energy. To
    // simulate constant kinetic energy, we give particles a little bump when they collide with the wall if the total
    // kinetic of the system is currently lower than it was at the start of the simulation (kineticEnergyDeficiency =
    // initialKineticEnergy - currentTotalKineticEnergy)

    float randomNumber = getRandomFloat() / 2.f;
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

    float v = abs(d2.getVelocity() - d1.getVelocity());
    const auto& m1 = d1.getType().getMass();
    const auto& m2 = d2.getType().getMass();

    float impulse = v * (e + 1) / (1.f / m1 + 1.f / m2);
    sf::Vector2f rVec = d2.getPosition() - d1.getPosition();
    sf::Vector2f nVec = rVec / abs(rVec);

    d1.accelerate(-impulse / m1 * nVec);
    d2.accelerate(impulse / m2 * nVec);
}

float getRandomFloat()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> distribution(0, 1);

    return distribution(gen);
}

} // namespace MathUtils