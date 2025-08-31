#ifndef CELL_HPP
#define CELL_HPP

#include "Types.hpp"

#include <SFML/System/Time.hpp>

#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

namespace cell
{

class Disc;
class ReactionEngine;
class CollisionDetector;
class CollisionHandler;

class Cell
{
public:
    Cell(ReactionEngine& reactionEngine, CollisionDetector& collisionDetector, CollisionHandler& collisionHandler,
         SimulationTimeStepProvider simulationTimeStepProvider, DiscTypeResolver discTypeResolver,
         Dimensions dimensions, std::vector<Disc>&& discs);

    /**
     * @brief Advances the simulation by a single time step
     */
    void update();

    /**
     * @returns all discs currently part of this cell
     */
    const std::vector<Disc>& getDiscs() const;

    /**
     * @returns The initial kinetic energy of all discs in this cell after `reinitialize()` was called
     */
    double getInitialKineticEnergy() const;

    /**
     * @brief The current kinetc energy of all discs in this cell
     */
    double getCurrentKineticEnergy() const;

private:
    /**
     * @brief Removed all discs that were marked as destroyed (i. e. after decomposition or combination reactions) and
     * calculates the current kinetic energy based on the discs that are still in the cell
     */
    void removeDestroyedDiscs();

private:
    ReactionEngine& reactionEngine_;
    CollisionDetector& collisionDetector_;
    CollisionHandler& collisionHandler_;

    SimulationTimeStepProvider simulationTimeStepProvider_;
    DiscTypeResolver discTypeResolver_;

    std::vector<Disc> discs_;

    double width_;
    double height_;

    // TODO remove, the simulation doesn't care about this
    double initialKineticEnergy_ = 0;
    double currentKineticEnergy_ = 0;
};

} // namespace cell

#endif /* CELL_HPP */
