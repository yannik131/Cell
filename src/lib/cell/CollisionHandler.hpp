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
        Disc* disc = nullptr;
        PhysicalObject* obj2 = nullptr;

        double invMass1 = 0, invMass2 = 0;
        double effMass = 0;

        Vector2d normal;
        double penetration = 0;

        double impulseChange = 0;

        bool skipCollision = false;
    };

public:
    explicit CollisionHandler(const DiscTypeRegistry& discTypeRegistry,
                              const MembraneTypeRegistry& membraneTypeRegistry);
    void resolveCollisions(const std::vector<CollisionDetector::Collision>& collisions) const;

private:
    CollisionContext calculateCollisionContext(const CollisionDetector::Collision& collision) const;
    void handleCollision(const CollisionDetector::Collision& collision) const;

private:
    const DiscTypeRegistry& discTypeRegistry_;
    const MembraneTypeRegistry& membraneTypeRegistry_;
};

} // namespace cell

#endif /* E0D5D573_BFE6_46C5_8B59_848C70859E48_HPP */
