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
    void resolveCollisions(const std::vector<CollisionDetector::Collision>& collisions,
                           CollisionDetector::Params params, double dt);

private:
    const DiscTypeRegistry& discTypeRegistry_;
    const MembraneTypeRegistry& membraneTypeRegistry_;
};

} // namespace cell

#endif /* E0D5D573_BFE6_46C5_8B59_848C70859E48_HPP */
