#include "MathUtils.hpp"
#include "GlobalSettings.hpp"
#include "NanoflannAdapter.hpp"

#include "nanoflann.hpp"
#include <glog/logging.h>

#include <numeric>
#include <random>

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
        d1->markChanged();
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

        // TODO This looks wrong: Write down the math for all reaction types in latex with nice graphics
        // Edit: It's not wrong, but latex is still a good idea.
        // m1*v1^2 = m2*v2^2 <-> v2 = sqrt(m1/m2)*v1
        d1->scaleVelocity(std::sqrt(d1->getType().getMass() / reaction.getProduct1().getMass()));
        d1->setType(reaction.getProduct1());
        d1->markChanged();

        d2->scaleVelocity(std::sqrt(d2->getType().getMass() / reaction.getProduct2().getMass()));
        d2->setType(reaction.getProduct2());
        d2->markChanged();

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

        const float MassFraction =
            d1->getType().getMass() / (reaction.getProduct1().getMass() + reaction.getProduct2().getMass());
        const float Factor = std::sqrt(2) / 2 * MassFraction;
        const auto& v = d1->getVelocity();
        const auto& r = d1->getPosition();
        const float vAbs = abs(v);

        Disc product1(reaction.getProduct1());
        product1.setVelocity(Factor * sf::Vector2f{v.x - v.y, v.x + v.y});
        product1.setPosition(r + product1.getVelocity() / vAbs);

        Disc product2(reaction.getProduct2());
        product2.setVelocity(Factor * sf::Vector2f{v.x + v.y, v.y - v.x});
        product2.setPosition(r + product2.getVelocity() / vAbs);

        correctOverlap(product1, product2);

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

    for (auto& disc : discs)
    {
        if (disc.isMarkedDestroyed())
            continue;

        discsInRadius.clear();
        const float maxCollisionDistance = disc.getType().getRadius() + maxRadius;

        // This is the most time consuming part of the whole application, next to the index build in the KDTree
        // constructor
        kdtree.radiusSearch(&disc.getPosition().x, maxCollisionDistance * maxCollisionDistance, discsInRadius,
                            searchParams);

        for (size_t i = 0; i < discsInRadius.size(); ++i)
        {
            if (discsInRadius[i].second == 0)
                continue;

            auto& otherDisc = discs[discsInRadius[i].first];

            const float radiusSum = disc.getType().getRadius() + otherDisc.getType().getRadius();

            if (discsInRadius[i].second <= radiusSum * radiusSum)
            {
                Disc* p1 = &disc;
                Disc* p2 = &otherDisc;

                if (p2 < p1)
                    std::swap(p1, p2);

                collidingDiscs.insert(std::make_pair(p1, p2));
                break;
            }
        }
    }

    return collidingDiscs;
}

std::map<DiscType, int> handleDiscCollisions(const std::set<std::pair<Disc*, Disc*>>& collidingDiscs)
{
    std::map<DiscType, int> collisionCounts;

    const float frictionCoefficient = GlobalSettings::getSettings().frictionCoefficient;

    for (const auto& [p1, p2] : collidingDiscs)
    {
        const auto& [normal, distance, overlap] = correctOverlap(*p1, *p2);

        // No overlap -> no collision
        if (overlap <= 0)
            continue;

        ++collisionCounts[p1->getType()];
        ++collisionCounts[p2->getType()];

        // Don't handle collision if reaction occured
        if (combinationReaction(p1, p2))
            continue;

        // Tangential vector of the collision
        sf::Vector2f tangent(-normal.y, normal.x);

        // Relative velocity
        sf::Vector2f relativeVelocity = p2->getVelocity() - p1->getVelocity();
        float velocityAlongNormal = relativeVelocity.x * normal.x + relativeVelocity.y * normal.y;
        float velocityAlongTangent = relativeVelocity.x * tangent.x + relativeVelocity.y * tangent.y;

        // Coefficient of restitution (elasticity of the collision)
        const float e = 1.f; // Fully elastic

        const auto &m1 = p1->getType().getMass(), m2 = p2->getType().getMass();
        // Impulse exchange in the normal direction
        float jNormal = -(1 + e) * velocityAlongNormal;
        jNormal /= (1 / m1 + 1 / m2);

        // Impulse exchange in the tangential direction (considering friction)
        float jTangent = -frictionCoefficient * velocityAlongTangent;
        jTangent /= (1 / m1 + 1 / m2);

        // Total impulse
        sf::Vector2f impulse = jNormal * normal + jTangent * tangent;

        // Apply the impulse
        p1->accelerate(-impulse / m1);
        p2->accelerate(impulse / m2);

        if (exchangeReaction(p1, p2))
            correctOverlap(*p1, *p2);
    }

    return collisionCounts;
}

float handleWorldBoundCollision(Disc& disc, const sf::Vector2f& bounds, float kineticEnergyDeficiency)
{
    // https://hermann-baum.de/bouncing-balls/
    const auto& r = disc.getType().getRadius();
    const auto& pos = disc.getPosition();
    const auto& v = disc.getVelocity();

    float dx = 0, dy = 0;
    bool collided = false;

    if (pos.x < r)
    {
        dx = r - pos.x + 1;
        dy = dx * v.y / v.x;

        disc.negateXVelocity();
        collided = true;
    }
    else if (pos.x > bounds.x - r)
    {
        dx = -(pos.x + r - bounds.x + 1);
        dy = -(dx * v.y / v.x);

        disc.negateXVelocity();
        collided = true;
    }

    if (pos.y < r)
    {
        dy = r - pos.y + 1;
        dx = dy * v.x / v.y;

        disc.negateYVelocity();
        collided = true;
    }
    else if (pos.y > bounds.y - r)
    {
        dy = -(pos.y + r - bounds.y + 1);
        dx = -(dy * v.x / v.y);

        disc.negateYVelocity();
        collided = true;
    }

    if (!collided)
        return 0.f;

    disc.move({dx, dy});

    // Combination reactions are treated as inelastic collisions, so they don't conserve total kinetic energy. To
    // simulate constant kinetic energy, we give particles a little bump when they collide with the wall if the total
    // kinetic of the system is currently lower than it was at the start of the simulation (kineticEnergyDeficiency =
    // initialKineticEnergy - currentTotalKineticEnergy)
    // TODO really oughta plot the total kinetic energy, total impulse and start writing tests, I think this amount is
    // too small
    float randomNumber = getRandomFloat() / 10.f;
    if (kineticEnergyDeficiency <= 0)
        return 0.f; // If we have more than we had at the start, we just wait for the inelastic collisions to drain it

    float kineticEnergyBefore = disc.getKineticEnergy();
    disc.scaleVelocity(1.f + randomNumber);

    return disc.getKineticEnergy() - kineticEnergyBefore;
}

float abs(const sf::Vector2f& vec)
{
    return std::hypot(vec.x, vec.y);
}

std::tuple<sf::Vector2f, float, float> correctOverlap(Disc& d1, Disc& d2)
{
    const sf::Vector2f& diff = d2.getPosition() - d1.getPosition();
    float distance = abs(diff);
    const sf::Vector2f& normal = diff / distance;
    float overlap = d1.getType().getRadius() + d2.getType().getRadius() - distance;

    if (overlap > 0)
    {
        d1.move(-overlap * normal / 2.0f);
        d2.move(overlap * normal / 2.0f);
    }

    return {normal, distance, overlap};
}

float getRandomFloat()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> distribution(0, 1);

    return distribution(gen);
}

} // namespace MathUtils