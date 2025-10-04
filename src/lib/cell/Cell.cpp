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

Cell::Cell(Membrane&& membrane, std::vector<Membrane>& membranes, SimulationContext simulationContext)
    : Compartment(nullptr, std::move(membrane),
                  sortedDescendingByRadius(membranes, simulationContext.membraneTypeRegistry),
                  std::move(simulationContext))
{
    if (!membranes.empty())
        throw ExceptionWithLocation(
            std::to_string(membranes.size()) +
            " membrane(s) were not fully contained by others. Intersecting membranes are not allowed.");
}

double Cell::getInitialKineticEnergy() const
{
    return initialKineticEnergy_;
}

double Cell::getCurrentKineticEnergy() const
{
    return currentKineticEnergy_;
}

std::vector<Membrane>& Cell::sortedDescendingByRadius(std::vector<Membrane>& membranes,
                                                      const MembraneTypeRegistry& membraneTypeRegistry)
{
    std::sort(membranes.begin(), membranes.end(),
              [&](const Membrane& lhs, const Membrane& rhs)
              {
                  return membraneTypeRegistry.getByID(lhs.getMembraneTypeID()).getRadius() >
                         membraneTypeRegistry.getByID(rhs.getMembraneTypeID()).getRadius();
              });

    return membranes;
}

} // namespace cell