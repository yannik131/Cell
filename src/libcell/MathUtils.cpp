#include "MathUtils.hpp"
#include "GlobalSettings.hpp"
#include "NanoflannAdapter.hpp"

#include "nanoflann.hpp"

namespace MathUtils
{

typedef nanoflann::L2_Simple_Adaptor<float, NanoflannAdapter> AdapterType;
typedef nanoflann::KDTreeSingleIndexAdaptor<AdapterType, NanoflannAdapter, 2> KDTree;

std::set<std::pair<Disc*, Disc*>> findCollidingDiscs(std::vector<Disc>& discs, int maxRadius)
{
    NanoflannAdapter adapter(discs);
    KDTree kdtree(2, adapter);
    const nanoflann::SearchParameters searchParams(0, false);

    std::set<std::pair<Disc*, Disc*>> collidingDiscs;
    static std::vector<nanoflann::ResultItem<uint32_t, float>> discsInRadius;

    for (auto& disc : discs)
    {
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
                auto p2 = &otherDisc;
                if (p2 < p1)
                    std::swap(p1, p2);
                collidingDiscs.insert(std::make_pair(p1, p2));
                break;
            }
        }
    }

    return collidingDiscs;
}

int handleDiscCollisions(const std::set<std::pair<Disc*, Disc*>>& collidingDiscs, const sf::Time& dt)
{
    int collisionCount = 0;
    const float frictionCoefficient = GlobalSettings::getSettings().frictionCoefficient;

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
        float distance = std::sqrt(normal.x * normal.x + normal.y * normal.y);
        normal /= distance;

        // Tangential vector of the collision
        sf::Vector2f tangent(-normal.y, normal.x);

        // Relative velocity
        sf::Vector2f relativeVelocity = v2 - v1;
        float velocityAlongNormal = relativeVelocity.x * normal.x + relativeVelocity.y * normal.y;
        float velocityAlongTangent = relativeVelocity.x * tangent.x + relativeVelocity.y * tangent.y;

        // If the particles are moving away from each other, no collision
        if (velocityAlongNormal > 0)
            continue;

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

        // Correct positions to avoid overlaps
        float overlap = (r1 + r2) - distance;
        pos1 -= overlap * normal / 2.0f;
        pos2 += overlap * normal / 2.0f;

        ++collisionCount;
    }

    return collisionCount;
}

void handleWorldBoundCollision(Disc& disc, const sf::Vector2f& bounds)
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
}

} // namespace MathUtils