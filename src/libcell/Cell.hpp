#ifndef CELL_HPP
#define CELL_HPP

#include "CellState.hpp"

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
    Cell(const ReactionEngine* reactionEngine, const CollisionDetector* collisionDetector,
         const CollisionHandler* collisionHandler);

    void setState(CellState&& state);

    /**
     * @brief Advances the simulation by a single time step
     */
    void update(const sf::Time& dt);

    /**
     * @returns the collision counts for all disc types in the simulation and sets them to 0
     */
    DiscTypeMap<int> getAndResetCollisionCount();

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
    DiscTypeMap<int> collisionCounts_;

    std::vector<Disc> newDiscs_;

    std::unique_ptr<CellState> state_;

    const ReactionEngine* reactionEngine_;
    const CollisionDetector* collisionDetector_;
    const CollisionHandler* collisionHandler_;
};

} // namespace cell

#endif /* CELL_HPP */
