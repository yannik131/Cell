#include "Cell.hpp"
#include "CollisionDetector.hpp"
#include "CollisionHandler.hpp"
#include "Compartment.hpp"
#include "Disc.hpp"
#include "MathUtils.hpp"
#include "Membrane.hpp"
#include "ReactionEngine.hpp"
#include "Settings.hpp"

#include <algorithm>

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