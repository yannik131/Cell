#ifndef CELLSTATE_HPP
#define CELLSTATE_HPP

#include <vector>

#include "Vector2d.hpp"
#include "types/DiscType.hpp"

namespace cell
{

class Disc;
class Membrane;

class CellState
{
public:
    CellState() = default;

    void update(double dt);

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
