#ifndef COLLISIONDETECTOR_HPP
#define COLLISIONDETECTOR_HPP

#include <set>
#include <utility>
#include <vector>

namespace cell
{

class Disc;
class Membrane;

class CollisionDetector
{
public:
    CollisionDetector();

    /**
     * @brief Finds all discs and membranes in the given vectors that overlap using nanoflann. Note that this uses the
     * maximum radius in the disc type distribution from the settings
     * @param discs The vector of discs
     * @param maxRadius The largest radius of all discs in the simulation. Used for the radius search in the kd tree to
     * find candidates for collision
     */
    void detectCollisions(std::vector<Disc>& discs, std::vector<Membrane>& membranes);

    void updateMaxDiscRadius();

    double getMaxDiscRadius() const;

    std::set<std::pair<Disc*, Disc*>> getDiscDiscCollisions() const;
    std::set<std::pair<Disc*, Membrane*>> getDiscMembraneCollisions() const;

private:
    std::set<std::pair<Disc*, Disc*>> detectDiscDiscCollisions(std::vector<Disc>& discs);
    std::set<std::pair<Disc*, Membrane*>> detectDiscMembraneCollisions(std::vector<Disc>& discs,
                                                                       std::vector<Membrane>& membranes);

private:
    std::set<std::pair<Disc*, Disc*>> discDiscCollisions_;
    std::set<std::pair<Disc*, Membrane*>> discMembraneCollisions_;
    double maxDiscRadius_ = 0;
};

} // namespace cell

#endif /* COLLISIONDETECTOR_HPP */
