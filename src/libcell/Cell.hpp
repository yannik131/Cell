#ifndef CELL_HPP
#define CELL_HPP

#include "CellState.hpp"
#include "Disc.hpp"
#include "NanoflannAdapter.hpp"
#include "ReactionEngine.hpp"

#include <SFML/System/Time.hpp>

#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

namespace cell
{

/**
 * @brief Represents a single cell with bounds and discs that can move around, collide and react
 * @note Usage:
 * ```cpp
 * Cell cell;
 * GlobalSettings::get().setCellSize(500, 500);
 * cell.reinitialize(); // Populate the cell
 * cell.update(sf::milliseconds(1)); // Advance the simulation
 * // Do whatever
 */
class Cell
{
public:
    Cell(const ReactionEngine& reactionEngine);

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
    std::vector<Disc> discs_;
    DiscTypeMap<int> collisionCounts_;

    std::vector<Disc> newDiscs_;

    std::unique_ptr<CellState> state_;

    const ReactionEngine* reactionEngine_;
};

} // namespace cell

#endif /* CELL_HPP */
