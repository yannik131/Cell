#include "MathUtils.hpp"
#include "GlobalSettings.hpp"
#include "NanoflannAdapter.hpp"

#include "nanoflann.hpp"
#include <glog/logging.h>

#include <numeric>
#include <random>

namespace MathUtils
{

static std::random_device rd;
static std::mt19937 gen(rd());
static std::uniform_real_distribution<float> distribution(0, 1);

typedef nanoflann::L2_Simple_Adaptor<float, NanoflannAdapter> AdapterType;
typedef nanoflann::KDTreeSingleIndexAdaptor<AdapterType, NanoflannAdapter, 2> KDTree;

void decompositionReaction(Disc* d1, std::vector<Disc>& newDiscs)
{
    const auto& decompositionReactionTable = GlobalSettings::getSettings().decompositionReactions_;
    const float& simulationTimeStep = GlobalSettings::getSettings().simulationTimeStep_.asSeconds();

    const auto& iter = decompositionReactionTable.find(d1->getType());
    if (iter == decompositionReactionTable.end())
        return;

    const auto& possibleReactions = iter->second;
    float randomNumber = distribution(gen);
    for (const auto& reaction : possibleReactions)
    {
        if (randomNumber > reaction.probability_ * simulationTimeStep)
            continue;

        const float MassFraction = d1->getType().mass_ / (reaction.product1_.mass_ + reaction.product2_.mass_);
        const float Factor = std::sqrt(2) / 2 * MassFraction;
        const auto& v = d1->getVelocity();
        const auto& r = d1->getPosition();
        const float vAbs = std::hypot(v.x, v.y);

        Disc product1(reaction.product1_);
        product1.setVelocity(Factor * sf::Vector2f{v.x - v.y, v.x + v.y});
        product1.setPosition(r + product1.getVelocity() / vAbs);

        Disc product2(reaction.product2_);
        product2.setVelocity(Factor * sf::Vector2f{v.x + v.y, v.y - v.x});
        product2.setPosition(r + product2.getVelocity() / vAbs);

        sf::Vector2f normal = product2.getPosition() - product1.getPosition();
        float distance = std::hypot(normal.x, normal.y);
        normal /= distance;

        float overlap = (product1.getType().radius_ + product2.getType().radius_) - distance + 1;
        product1.move(-overlap * normal / 2.0f);
        product2.move(overlap * normal / 2.0f);

        newDiscs.push_back(std::move(product1));
        newDiscs.push_back(std::move(product2));
        d1->markDestroyed();

        return;
    }

    return;
}

bool combinationReaction(Disc* d1, Disc* d2)
{
    const auto& combinationReactionTable = GlobalSettings::getSettings().combinationReactions_;
    auto iter = combinationReactionTable.find(std::make_pair(d2->getType(), d1->getType()));

    if (iter == combinationReactionTable.end())
        return false;

    const auto& possibleReactions = iter->second;
    float randomNumber = distribution(gen);
    for (const auto& reaction : possibleReactions)
    {
        if (randomNumber > reaction.probability_)
            continue;

        const auto& resultType = reaction.product1_;

        // For reactions of type A + B -> C, we keep the one closer in size to C and destroy the other
        if (std::abs(resultType.radius_ - d1->getType().radius_) > std::abs(resultType.radius_ - d2->getType().radius_))
            std::swap(d1, d2);

        d1->setType(resultType);
        d1->markChanged();
        d1->setVelocity((d1->getType().mass_ * d1->getVelocity() + d2->getType().mass_ * d2->getVelocity()) /
                        resultType.mass_);

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
    float randomNumber = distribution(gen);
    for (const auto& reaction : possibleReactions)
    {
        if (randomNumber > reaction.probability_)
            continue;

        // m1*v1^2 = m2*v2^2 <-> v2 = sqrt(m1/m2)*v1
        d1->scaleVelocity(std::sqrt(d1->getType().mass_ / reaction.product1_.mass_));
        d1->setType(reaction.product1_);
        d1->markChanged();

        d2->scaleVelocity(std::sqrt(d2->getType().mass_ / reaction.product2_.mass_));
        d2->setType(reaction.product2_);
        d2->markChanged();

        return true;
    }

    return false;
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
        const float maxCollisionDistance = disc.getType().radius_ + maxRadius;

        // This is the most time consuming part of the whole application, next to the index build in the KDTree
        // constructor
        kdtree.radiusSearch(&disc.getPosition().x, maxCollisionDistance * maxCollisionDistance, discsInRadius,
                            searchParams);

        for (size_t i = 0; i < discsInRadius.size(); ++i)
        {
            if (discsInRadius[i].second == 0)
                continue;

            auto& otherDisc = discs[discsInRadius[i].first];

            const float radiusSum = disc.getType().radius_ + otherDisc.getType().radius_;

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

int handleDiscCollisions(const std::set<std::pair<Disc*, Disc*>>& collidingDiscs)
{
    int collisionCount = 0;
    const float frictionCoefficient = GlobalSettings::getSettings().frictionCoefficient;

    for (const auto& [p1, p2] : collidingDiscs)
    {
        // Normal vector of the collision
        sf::Vector2f normal = p2->getPosition() - p1->getPosition();
        float distance = std::hypot(normal.x, normal.y);
        normal /= distance;

        // Correct positions to avoid overlaps
        float overlap = p1->getType().radius_ + p2->getType().radius_ - distance;

        p1->move(-overlap * normal / 2.0f);
        p2->move(overlap * normal / 2.0f);

        // No overlap -> no collision
        if (overlap <= 0)
            continue;

        ++collisionCount;

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

        const auto &m1 = p1->getType().mass_, m2 = p2->getType().mass_;
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
        {
            // Might have overlap now if the discs increased in size
            // TODO maybe write a class that gives these values so we don't have to copy paste code? Could take 2 discs
            // as an argument and provide methods for overlap correction and retrieval of intermediate values like the
            // normal and distance for further use
            normal = p2->getPosition() - p1->getPosition();
            distance = std::hypot(normal.x, normal.y);
            overlap = p1->getType().radius_ + p2->getType().radius_ - distance;

            p1->move(-overlap * normal / 2.0f);
            p2->move(overlap * normal / 2.0f);
        }
    }

    return collisionCount;
}

float handleWorldBoundCollision(Disc& disc, const sf::Vector2f& bounds, float kineticEnergyDeficiency)
{
    // https://hermann-baum.de/bouncing-balls/
    const auto& r = disc.getType().radius_;
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
    float randomNumber = distribution(gen) / 10.f;
    if (kineticEnergyDeficiency <= 0)
        return 0.f; // If we have more than we had at the start, we just wait for the inelastic collisions to drain it

    float kineticEnergyBefore = disc.getKineticEnergy();
    disc.scaleVelocity(1 + randomNumber);

    return disc.getKineticEnergy() - kineticEnergyBefore;
}

float abs(const sf::Vector2f& vec)
{
    return std::hypot(vec.x, vec.y);
}

} // namespace MathUtils