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
public:
    explicit CollisionHandler(const DiscTypeRegistry& discTypeRegistry,
                              const MembraneTypeRegistry& membraneTypeRegistry);

    void
    calculateDiscDiscCollisionResponse(std::vector<CollisionDetector::DiscDiscCollision>& discDiscCollisions) const;
    void calculateDiscChildMembraneCollisionResponse(
        std::vector<CollisionDetector::DiscChildMembraneCollision>& discChildMembraneCollisions) const;
    void calculateDiscContainingMembraneCollisionResponse(
        std::vector<CollisionDetector::DiscContainingMembraneCollision>& discContainingMembraneCollisions) const;

private:
    void updateVelocitiesDiscDiscCollision(CollisionDetector::DiscDiscCollision& collision) const;
    void updateVelocitiesDiscChildMembraneCollision(CollisionDetector::DiscChildMembraneCollision& collision) const;
    void updateVelocitiesDiscContainingMembraneCollision(
        CollisionDetector::DiscContainingMembraneCollision& collision) const;

private:
    const DiscTypeRegistry& discTypeRegistry_;
    const MembraneTypeRegistry& membraneTypeRegistry_;
};

} // namespace cell

#endif /* E0D5D573_BFE6_46C5_8B59_848C70859E48_HPP */
