#ifndef CED957EE_7402_4CC8_9E3D_517440690039_HPP
#define CED957EE_7402_4CC8_9E3D_517440690039_HPP

#include "Types.hpp"

namespace cell
{

class ReactionEngine;
class CollisionDetector;
class CollisionHandler;

struct SimulationContext
{
    const DiscTypeRegistry& discTypeRegistry;
    const MembraneTypeRegistry& membraneTypeRegistry;
    const ReactionEngine& reactionEngine;
    CollisionDetector& collisionDetector;
    const CollisionHandler& collisionHandler;
};

} // namespace cell

#endif /* CED957EE_7402_4CC8_9E3D_517440690039_HPP */
