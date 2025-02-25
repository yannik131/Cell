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

std::vector<Disc> decomposeDiscs(std::vector<Disc>& discs)
{
    const auto& decompositionReactionTable = GlobalSettings::getSettings().decompositionReactionTable_;
    const float& simulationTimeStep = GlobalSettings::getSettings().simulationTimeStep_.asSeconds();
    std::vector<Disc> newDiscs;

    for (auto& disc : discs)
    {
        const auto& iter = decompositionReactionTable.find(disc.type_);
        if (iter == decompositionReactionTable.end())
            continue;

        const auto& possibleReactions = iter->second;
        float randomNumber = distribution(gen);
        for (const auto& [resultTypePair, probability] : possibleReactions)
        {
            if (randomNumber > probability * simulationTimeStep)
                continue;

            // We will put both resulting discs in the same position as the old one and let the collision algorithm do
            // the separation for us
            // This way we also account for immediate re-formation

            const float MassFraction = disc.type_.mass_ / (resultTypePair.first.mass_ + resultTypePair.second.mass_);
            const float Factor = std::sqrt(2) / 2 * MassFraction;
            const auto& v = disc.velocity_;
            const auto& r = disc.position_;
            const float vAbs = std::hypot(v.x, v.y);

            Disc product1(resultTypePair.first);
            product1.velocity_ = Factor * sf::Vector2f{v.x - v.y, v.x + v.y};
            product1.position_ = r + product1.velocity_ / vAbs;

            Disc product2(resultTypePair.second);
            product2.velocity_ = Factor * sf::Vector2f{v.x + v.y, v.y - v.x};
            product2.position_ = r + product2.velocity_ / vAbs;

            sf::Vector2f normal = product2.position_ - product1.position_;
            float distance = std::hypot(normal.x, normal.y);
            normal /= distance;

            float overlap = (product1.type_.radius_ + product2.type_.radius_) - distance;
            product1.position_ -= overlap * normal / 2.0f;
            product2.position_ += overlap * normal / 2.0f;

            newDiscs.push_back(std::move(product1));
            newDiscs.push_back(std::move(product2));
            disc.destroyed_ = true;

            break;
        }
    }

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
        // TODO Maybe find a better way? We handle decomposition reaction before this to account for immediate
        // re-combination
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
    const auto& reactionTable = GlobalSettings::getSettings().combinationReactionTable_;

    // DeepSeek-generated
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

        auto iter = reactionTable.find(p2->type_ < p1->type_ ? std::make_pair(p2->type_, p1->type_)
                                                             : std::make_pair(p1->type_, p2->type_));
        bool reactionOccured = false;
        if (iter != reactionTable.end())
        {
            const auto& possibleReactions = iter->second;
            float randomNumber = distribution(gen);
            for (const auto& [resultType, probability] : possibleReactions)
            {
                if (randomNumber > probability)
                    continue;

                if (std::abs(resultType.radius_ - p1->type_.radius_) < std::abs(resultType.radius_ - p2->type_.radius_))
                {
                    p1->type_ = resultType;
                    p1->changed_ = true;
                    p2->destroyed_ = true;
                    v1 = (m1 * v1 + m2 * v2) / resultType.mass_;
                }
                else
                {
                    p2->type_ = resultType;
                    p2->changed_ = true;
                    p1->destroyed_ = true;
                    v2 = (m1 * v1 + m2 * v2) / resultType.mass_;
                }
                reactionOccured = true;
                break;
            }
        }

        if (!reactionOccured)
        {
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
        }

        ++collisionCount;
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

    if (pos.x < r)
    {
        dx = r - pos.x + 1;
        dy = dx * v.y / v.x;

        pos += {dx, dy};
        v.x = -v.x;
    }
    else if (pos.x > bounds.x - r)
    {
        dx = -(pos.x + r - bounds.x + 1);
        dy = -(dx * v.y / v.x);

        pos += {dx, dy};
        v.x = -v.x;
    }

    if (pos.y < r)
    {
        dy = r - pos.y + 1;
        dx = dy * v.x / v.y;

        pos += {dx, dy};
        v.y = -v.y;
    }
    else if (pos.y > bounds.y - r)
    {
        dy = -(pos.y + r - bounds.y + 1);
        dx = -(dy * v.x / v.y);

        pos += {dx, dy};
        v.y = -v.y;
    }

    float randomNumber = distribution(gen) / 10.f;
    if (kineticEnergyDeficiency <= 0)
        randomNumber = -randomNumber;

    float kineticEnergyBefore = disc.getKineticEnergy();
    v *= 1 + randomNumber;

    return disc.getKineticEnergy() - kineticEnergyBefore;
}

} // namespace MathUtils