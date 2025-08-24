#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include "Cell.hpp"
#include "SimulationContext.hpp"

namespace cell
{

class Simulation
{
public:
    Simulation() = default;

private:
    SimulationContext simulationContext_;
    Cell cell;
};

} // namespace cell

#endif /* SIMULATION_HPP */
