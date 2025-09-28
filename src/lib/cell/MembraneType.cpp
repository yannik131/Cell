#include "MembraneType.hpp"

namespace cell
{

MembraneType::MembraneType(std::string name, double radius, MembraneType::PermeabilityMap permeabilityMap)
    : name_(std::move(name))
    , radius_(radius)
    , permeabilityMap_(std::move(permeabilityMap))
{
}

MembraneType::Permeability MembraneType::getPermeabilityFor(const DiscTypeID& discTypeID) const
{
    auto iter = permeabilityMap_.find(discTypeID);
    if (iter == permeabilityMap_.end())
        return Permeability::None;

    return iter->second;
}

const std::string& MembraneType::getName() const
{
    return name_;
}

double MembraneType::getRadius() const
{
    return radius_;
}

} // namespace cell