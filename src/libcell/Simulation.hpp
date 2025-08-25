#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include "Cell.hpp"
#include "SimulationContext.hpp"

namespace cell
{

class Simulation
{
public:
    Simulation();

    void run();

private:
    SimulationContext simulationContext_;
    Cell cell_;
};

} // namespace cell

#endif /* SIMULATION_HPP */
