#include "Simulation.hpp"
#include "CellState.hpp"
#include "MathUtils.hpp"

namespace cell
{

Simulation::Simulation()
    : cell_(simulationContext_.getReactionEngine(), simulationContext_.getCollisionDetector(),
            simulationContext_.getCollisionHandler())
{
}

void Simulation::run()
{
    CellState cellState(simulationContext_.getDiscTypeResolver(), simulationContext_.getMaxRadiusProvider());
    cellState.randomizeUsingDiscTypeDistribution();

    cell_.setState(std::move(cellState));

    while (true)
    {
        cell_.update(simulationContext_.getSimulationTimeStep() * simulationContext_.getSimulationTimeScale());
    }
}

} // namespace cell