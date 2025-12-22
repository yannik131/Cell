#include "Cell.hpp"

namespace cell
{

Cell::Cell(Membrane membrane, SimulationContext simulationContext)
    : Compartment(nullptr, std::move(membrane), std::move(simulationContext))
{
}

} // namespace cell