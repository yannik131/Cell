#ifndef CELLSTATE_HPP
#define CELLSTATE_HPP

#include <vector>

#include "Vector2d.hpp"
#include "types/DiscType.hpp"

namespace cell
{

class Disc;
class Membrane;

/**
 * CellState should only contain a vector of discs and membranes and be a stupid class with getters and 2 options to
 * create the state: random using the settings, or using methods addDisc and addMembrane for more control without checks
 * (intersections might be on purpose for testing). Update and energy calculations should be in the cell class.
 */
class CellState
{
public:
    CellState() = default;

    void update(const sf::Time& dt);

    void addDisc(const Disc& disc);
    void addMembrane(const Membrane& membrane);

    /**
     * @brief Calculates a grid of starting positions for discs based on the largest radius of all disc types in the
     * settings. Doesn't check if the disc type with the largest radius can actually be created with the currently
     * defined set of reactions, though. Fills that grid with discs according to the current disc type distribution.
     */
    void randomize();

private:
    std::vector<sf::Vector2d> startPositions_;
    std::vector<Disc> discs_;
    std::vector<Membrane> membranes_;
    DiscType::map<int> collisionCounts_;
    double initialKineticEnergy_ = 0;
    double currentKineticEnergy_ = 0;
    std::vector<Disc> newDiscs_;
};

} // namespace cell

#endif /* CELLSTATE_HPP */
