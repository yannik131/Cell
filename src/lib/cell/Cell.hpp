#ifndef AE328161_6FB3_4EA0_8D38_CF7D51BEA90A_HPP
#define AE328161_6FB3_4EA0_8D38_CF7D51BEA90A_HPP

#include "Types.hpp"

#include <SFML/System/Time.hpp>

#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

namespace cell
{

class Disc;
class Membrane;
class Compartment;
class ReactionEngine;
class CollisionDetector;
class CollisionHandler;

class Cell
{
public:
    Cell(ReactionEngine& reactionEngine, CollisionDetector& collisionDetector, CollisionHandler& collisionHandler,
         const DiscTypeRegistry& discTypeRegistry, const MembraneTypeRegistry& membraneTypeRegistry,
         Dimensions dimensions, std::vector<Disc>&& discs, std::vector<Membrane>&& membranes);

    ~Cell();
    Cell(const Cell&);

    /**
     * @brief Advances the simulation by a single time step
     * @param dt Time step in seconds
     */
    void update(double dt);

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
    const DiscTypeRegistry& discTypeRegistry_;
    const MembraneTypeRegistry& membraneTypeRegistry_;

    double width_;
    double height_;

    std::vector<Disc> discs_;
    std::vector<Compartment> compartments_;

    // TODO remove, the simulation doesn't care about this
    double initialKineticEnergy_ = 0;
    double currentKineticEnergy_ = 0;
};

} // namespace cell

#endif /* AE328161_6FB3_4EA0_8D38_CF7D51BEA90A_HPP */
