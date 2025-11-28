#ifndef E0D5D573_BFE6_46C5_8B59_848C70859E48_HPP
#define E0D5D573_BFE6_46C5_8B59_848C70859E48_HPP

#include "CollisionDetector.hpp"
#include "Types.hpp"

#include <set>

namespace cell
{

class Disc;

class CollisionHandler
{
private:
    struct CollisionContext
    {
        Disc* disc;
        PhysicalObject* obj2;

        double invMass1, invMass2;
        double effMass;

        sf::Vector2d normal;
        double penetration;

        double impulseChange;

        bool skipCollision = false;
    };

public:
    explicit CollisionHandler(const DiscTypeRegistry& discTypeRegistry,
                              const MembraneTypeRegistry& membraneTypeRegistry);
    void resolveCollisions(const CollisionDetector::DetectedCollisions& detectedCollisions) const;

private:
    CollisionContext calculateCollisionContext(const CollisionDetector::Collision& collision) const;
    bool canGoThrough(Disc* disc, Membrane* membrane, CollisionDetector::CollisionType collisionType) const;

private:
    const DiscTypeRegistry& discTypeRegistry_;
    const MembraneTypeRegistry& membraneTypeRegistry_;
};

} // namespace cell

#endif /* E0D5D573_BFE6_46C5_8B59_848C70859E48_HPP */
