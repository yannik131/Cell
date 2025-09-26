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
    explicit CollisionHandler(DiscTypeResolver discTypeResolver);

    void calculateDiscDiscCollisionResponse(std::vector<std::pair<Disc*, Disc*>>& discDiscCollisions) const;

    void calculateDiscRectangleCollisionResponse(Disc& disc,
                                                 CollisionDetector::RectangleCollision& rectangleCollision) const;

    /**
     * Combination reactions are treated as inelastic collisions, so they don't conserve total kinetic energy. To
     * simulate constant kinetic energy, we give particles a little bump when they collide with the wall if the total
     * kinetic of the system is currently lower than it was at the start of the simulation (kineticEnergyDeficiency =
     * initialKineticEnergy - currentKineticEnergy)
     */
    double keepKineticEnergyConstant(Disc& disc, const CollisionDetector::RectangleCollision& collision,
                                     double deficiency) const;

private:
    double calculateOverlap(const Disc& d1, const Disc& d2) const;

    /**
     * @brief Given 2 circles, returns the earlier of the 2 times where they just started touching
     * @note If the overlap was calculated with `calculateOverlap()`, same ordering of parameters is required
     */
    double calculateTimeBeforeCollision(const Disc& d1, const Disc& d2) const;

    /**
     * @brief Given 2 colliding discs, calculates their new velocities based on a classical collision response
     */
    void updateVelocitiesAtCollision(Disc& d1, Disc& d2) const;

private:
    DiscTypeResolver discTypeResolver_;
};

} // namespace cell

#endif /* E0D5D573_BFE6_46C5_8B59_848C70859E48_HPP */
