#ifndef SIMULATIONCONTEXT_HPP
#define SIMULATIONCONTEXT_HPP

#include "CollisionDetector.hpp"
#include "CollisionHandler.hpp"
#include "DiscTypeRegistry.hpp"
#include "ReactionEngine.hpp"
#include "ReactionTable.hpp"

#include <SFML/System/Time.hpp>

namespace cell
{

class SimulationContext
{
public:
    SimulationContext();

    void setSimulationTimeStep(const sf::Time& simulationTimeStep);
    void setSimulationTimeScale(double simulationTimeScale);

    void run();

private:
    /**
     * @brief Time that passes between single simulation steps. Smaller value means more accurate collisions, but
     * requires more updates to advance the simulation in time. If this value is too small, the simulation might not be
     * able to keep up and start lagging
     */
    sf::Time simulationTimeStep_ = sf::microseconds(5000);

    /**
     * @brief Defines how many seconds should pass in real time for 1 second in the simulation.
     *
     * Example: If set to 2, we will advance the simulation by 2 seconds in 1 real time second, meaning we will (try to)
     * call the update() method of the world 2 * 1000/simulationTimeStep_ times per second
     */
    double simulationTimeScale_ = 1.0;

    DiscTypeRegistry discTypeRegistry_;
    DiscTypeResolver discTypeResolver_;
    ReactionTable reactionTable_;
    ReactionEngine reactionEngine_;
    CollisionDetector collisionDetector_;
    CollisionHandler collisionHandler_;
};

} // namespace cell

#endif /* SIMULATIONCONTEXT_HPP */
