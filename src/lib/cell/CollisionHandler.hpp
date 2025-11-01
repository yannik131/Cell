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

    void calculateDiscDiscCollisionResponse(std::vector<std::pair<Disc*, Disc*>>& discDiscCollisions) const;
    void calculateDiscMembraneCollisionResponse(std::vector<std::pair<Disc*, Membrane*>>& discMembraneCollisions) const;

    void calculateCircularBoundsCollisionResponse(Disc& disc, const sf::Vector2d& M, double Rm) const;

private:
    void updateVelocitiesDiscDiscCollision(Disc& d1, Disc& d2) const;
    void updateVelocitiesDiscMembraneCollision(Disc& disc, const Membrane& membrane) const;

private:
    const DiscTypeRegistry& discTypeRegistry_;
    const MembraneTypeRegistry& membraneTypeRegistry_;
};

} // namespace cell

#endif /* E0D5D573_BFE6_46C5_8B59_848C70859E48_HPP */
