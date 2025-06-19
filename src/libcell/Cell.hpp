#ifndef CELL_HPP
#define CELL_HPP

#include "Disc.hpp"
#include "NanoflannAdapter.hpp"

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
 * cell.setBounds(sf::Vector2f{500, 500});
 * cell.reinitialize(); // Populate the cell
 * cell.update(sf::milliseconds(1)); // Advance the simulation
 * // Do whatever
 */
class Cell
{
public:
    Cell();

    /**
     * @brief Advances the simulation by a single time step
     */
    void update(const sf::Time& dt);

    /**
     * @returns the collision counts for all disc types in the simulation and sets them to 0
     */
    DiscType::map<int> getAndResetCollisionCount();

    /**
     * @returns all discs currently part of this cell
     */
    const std::vector<Disc>& getDiscs() const;

    /**
     * @brief Initializes the starting positions and creates the discs in the cell according to the distribution in the
     * settings
     * @note Has to be called at least once before calling `update()`
     */
    void reinitialize();

    /**
     * @returns The initial kinetic energy of all discs in this cell after `reinitialize()` was called
     */
    float getInitialKineticEnergy() const;

    /**
     * @brief The current kinetc energy of all discs in this cell
     */
    float getCurrentKineticEnergy() const;

private:
    /**
     * @brief Creates discs within the boundaries at the starting positions
     */
    void buildScene();

    /**
     * @brief Calculates a grid of starting positions for discs based on the largest radius of all disc types in the
     * settings. Doesn't check if the disc type with the largest radius can actually be created with the currently
     * defined set of reactions, though.
     */
    void initializeStartPositions();

    /**
     * @brief Removed all discs that were marked as destroyed (i. e. after decomposition or combination reactions) and
     * calculates the current kinetic energy based on the discs that are still in the cell
     */
    void removeDestroyedDiscs();

private:
    std::vector<sf::Vector2f> startPositions_;
    std::vector<Disc> discs_;
    float maxRadius_{};
    DiscType::map<int> collisionCounts_;
    float initialKineticEnergy_{};
    float currentKineticEnergy_{};

    std::vector<Disc> newDiscs_;
};

} // namespace cell

#endif /* CELL_HPP */
