#include "types/MembraneType.hpp"
#include "ExceptionWithLocation.hpp"

namespace cell
{

void MembraneType::setPermeability(const DiscType& discType, PermeabilityType permeabilityType)
{
}

MembraneType::PermeabilityType MembraneType::getPermeability(const DiscType& discType) const
{
    return permeabilityMap_.at(discType);
}

double MembraneType::getThickness() const
{
    return thickness_;
}

void MembraneType::setThickness(double thickness)
{
    if (thickness <= 0)
        throw ExceptionWithLocation("Invalid thickness: " + std::to_string(thickness));

    thickness_ = thickness;
}

double MembraneType::getRadius() const
{
    return radius_;
}

void MembraneType::setRadius(double radius)
{
    if (radius <= 0)
        throw ExceptionWithLocation("Invalid radius: " + std::to_string(radius));

    radius_ = radius;
}

} // namespace cell