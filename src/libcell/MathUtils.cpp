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

    const auto& iter = decompositionReactionTable.find(d1->type_);
    if (iter == decompositionReactionTable.end())
        return;

    const auto& possibleReactions = iter->second;
    float randomNumber = distribution(gen);
    for (const auto& reaction : possibleReactions)
    {
        if (randomNumber > reaction.probability_ * simulationTimeStep)
            continue;

        const float MassFraction = d1->type_.mass_ / (reaction.product1_.mass_ + reaction.product2_.mass_);
        const float Factor = std::sqrt(2) / 2 * MassFraction;
        const auto& v = d1->velocity_;
        const auto& r = d1->position_;
        const float vAbs = std::hypot(v.x, v.y);

        Disc product1(reaction.product1_);
        product1.velocity_ = Factor * sf::Vector2f{v.x - v.y, v.x + v.y};
        product1.position_ = r + product1.velocity_ / vAbs;

        Disc product2(reaction.product2_);
        product2.velocity_ = Factor * sf::Vector2f{v.x + v.y, v.y - v.x};
        product2.position_ = r + product2.velocity_ / vAbs;

        sf::Vector2f normal = product2.position_ - product1.position_;
        float distance = std::hypot(normal.x, normal.y);
        normal /= distance;

        float overlap = (product1.type_.radius_ + product2.type_.radius_) - distance + 1;
        product1.position_ -= overlap * normal / 2.0f;
        product2.position_ += overlap * normal / 2.0f;

        newDiscs.push_back(std::move(product1));
        newDiscs.push_back(std::move(product2));
        d1->destroyed_ = true;

        return;
    }

    return;
}

bool combinationReaction(Disc* d1, Disc* d2)
{
    const auto& combinationReactionTable = GlobalSettings::getSettings().combinationReactions_;
    auto iter = combinationReactionTable.find(std::make_pair(d2->type_, d1->type_));

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
        if (std::abs(resultType.radius_ - d1->type_.radius_) < std::abs(resultType.radius_ - d2->type_.radius_))
        {
            d1->type_ = resultType;
            d1->changed_ = true;
            d2->destroyed_ = true;
            d1->velocity_ = (d1->type_.mass_ * d1->velocity_ + d2->type_.mass_ * d2->velocity_) / resultType.mass_;
        }
        else
        {
            d2->type_ = resultType;
            d2->changed_ = true;
            d1->destroyed_ = true;
            d2->velocity_ = (d1->type_.mass_ * d1->velocity_ + d2->type_.mass_ * d2->velocity_) / resultType.mass_;
        }

        return true;
    }

    return false;
}

bool exchangeReaction(Disc* d1, Disc* d2)
{
    // TODO Probabilities of exchange and combination reactions should together add up to 100%
    const auto& exchangeReactionTable = GlobalSettings::getSettings().combinationReactions_;
    auto iter = exchangeReactionTable.find(std::make_pair(d2->type_, d1->type_));

    if (iter == exchangeReactionTable.end())
        return false;

    const auto& possibleReactions = iter->second;
    float randomNumber = distribution(gen);
    for (const auto& reaction : possibleReactions)
    {
        if (randomNumber > reaction.probability_)
            continue;

        // m1*v1^2 = m2*v2^2 <-> v2 = sqrt(m1/m2)*v1
        d1->velocity_ *= std::sqrt(d1->type_.mass_ / reaction.product1_.mass_);
        d1->type_ = reaction.product1_;
        d1->changed_ = true;

        d2->velocity_ *= std::sqrt(d2->type_.mass_ / reaction.product2_.mass_);
        d2->type_ = reaction.product2_;
        d2->changed_ = true;

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
    for (const auto& disc : discs)
    {
        if (disc.destroyed_)
            break;
    }
    NanoflannAdapter adapter(discs);
    KDTree kdtree(2, adapter);
    const nanoflann::SearchParameters searchParams(0, false);

    std::set<std::pair<Disc*, Disc*>> collidingDiscs;
    static std::vector<nanoflann::ResultItem<uint32_t, float>> discsInRadius;

    for (auto& disc : discs)
    {
        if (disc.destroyed_)
            continue;

        discsInRadius.clear();
        const float maxCollisionDistance = disc.type_.radius_ + maxRadius;

        // This is the most time consuming part of the whole application, next to the index build in the KDTree
        // constructor
        kdtree.radiusSearch(&disc.position_.x, maxCollisionDistance * maxCollisionDistance, discsInRadius,
                            searchParams);

        for (size_t i = 0; i < discsInRadius.size(); ++i)
        {
            if (discsInRadius[i].second == 0)
                continue;

            auto& otherDisc = discs[discsInRadius[i].first];

            const float radiusSum = disc.type_.radius_ + otherDisc.type_.radius_;

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
        auto& pos1 = p1->position_;
        auto& pos2 = p2->position_;
        auto& v1 = p1->velocity_;
        auto& v2 = p2->velocity_;
        const auto& m1 = p1->type_.mass_;
        const auto& m2 = p2->type_.mass_;
        const auto& r1 = p1->type_.radius_;
        const auto& r2 = p2->type_.radius_;

        // Normal vector of the collision
        sf::Vector2f normal = pos2 - pos1;
        float distance = std::hypot(normal.x, normal.y);
        normal /= distance;

        // Correct positions to avoid overlaps
        float overlap = (r1 + r2) - distance;
        pos1 -= overlap * normal / 2.0f;
        pos2 += overlap * normal / 2.0f;

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
        sf::Vector2f relativeVelocity = v2 - v1;
        float velocityAlongNormal = relativeVelocity.x * normal.x + relativeVelocity.y * normal.y;
        float velocityAlongTangent = relativeVelocity.x * tangent.x + relativeVelocity.y * tangent.y;

        // Coefficient of restitution (elasticity of the collision)
        const float e = 1.f; // Fully elastic

        // Impulse exchange in the normal direction
        float jNormal = -(1 + e) * velocityAlongNormal;
        jNormal /= (1 / m1 + 1 / m2);

        // Impulse exchange in the tangential direction (considering friction)
        float jTangent = -frictionCoefficient * velocityAlongTangent;
        jTangent /= (1 / m1 + 1 / m2);

        // Total impulse
        sf::Vector2f impulse = jNormal * normal + jTangent * tangent;

        // Apply the impulse
        v1 -= impulse / m1;
        v2 += impulse / m2;

        if (exchangeReaction(p1, p2))
        {
            // Might have overlap now if the discs increased in size
            // TODO maybe write a class that gives these values so we don't have to copy paste code? Could take 2 discs
            // as an argument and provide methods for overlap correction and retrieval of intermediate values like the
            // normal and distance for further use
            normal = pos2 - pos1;
            distance = std::hypot(normal.x, normal.y);
            overlap = (r1 + r2) - distance;
            pos1 -= overlap * normal / 2.0f;
            pos2 += overlap * normal / 2.0f;
        }
    }

    return collisionCount;
}

float handleWorldBoundCollision(Disc& disc, const sf::Vector2f& bounds, float kineticEnergyDeficiency)
{
    // https://hermann-baum.de/bouncing-balls/
    const auto& r = disc.type_.radius_;
    auto& pos = disc.position_;
    auto& v = disc.velocity_;

    float dx, dy;
    bool collided = false;

    if (pos.x < r)
    {
        dx = r - pos.x + 1;
        dy = dx * v.y / v.x;

        pos += {dx, dy};
        v.x = -v.x;
        collided = true;
    }
    else if (pos.x > bounds.x - r)
    {
        dx = -(pos.x + r - bounds.x + 1);
        dy = -(dx * v.y / v.x);

        pos += {dx, dy};
        v.x = -v.x;
        collided = true;
    }

    if (pos.y < r)
    {
        dy = r - pos.y + 1;
        dx = dy * v.x / v.y;

        pos += {dx, dy};
        v.y = -v.y;
        collided = true;
    }
    else if (pos.y > bounds.y - r)
    {
        dy = -(pos.y + r - bounds.y + 1);
        dx = -(dy * v.x / v.y);

        pos += {dx, dy};
        v.y = -v.y;
        collided = true;
    }

    if (!collided)
        return 0.f;

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
    v *= 1 + randomNumber;

    return disc.getKineticEnergy() - kineticEnergyBefore;
}

} // namespace MathUtils