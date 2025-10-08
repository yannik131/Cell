#include "MembraneType.hpp"

namespace cell
{

MembraneType::MembraneType(std::string name, double radius, MembraneType::PermeabilityMap permeabilityMap)
    : name_(std::move(name))
    , permeabilityMap_(std::move(permeabilityMap))
    , radius_(radius)
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