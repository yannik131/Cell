#include "Membrane.hpp"
#include "ExceptionWithLocation.hpp"

namespace cell
{

void Membrane::setPermeability(const DiscType& discType, PermeabilityType permeabilityType)
{
}

Membrane::PermeabilityType Membrane::getPermeability(const DiscType& discType)
{
    return PermeabilityType();
}

double Membrane::getThickness() const
{
    return thickness_;
}

void Membrane::setThickness(double thickness)
{
    if (thickness <= 0)
        throw ExceptionWithLocation("Invalid thickness: " + std::to_string(thickness));

    thickness_ = thickness;
}

double Membrane::getRadius() const
{
    return radius_;
}

void Membrane::setRadius(double radius)
{
    if (radius <= 0)
        throw ExceptionWithLocation("Invalid radius: " + std::to_string(radius));

    radius_ = radius;
}

} // namespace cell