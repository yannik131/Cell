#include "Cell.hpp"

namespace cell
{

Cell::Cell(Membrane membrane, SimulationContext simulationContext)
    : Compartment(nullptr, std::move(membrane), std::move(simulationContext))
{
}

double Cell::getInitialKineticEnergy() const
{
    return initialKineticEnergy_;
}

double Cell::getCurrentKineticEnergy() const
{
    return currentKineticEnergy_;
}

} // namespace cell