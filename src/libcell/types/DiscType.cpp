#include "types/DiscType.hpp"
#include "ExceptionWithLocation.hpp"
#include "MathUtils.hpp"
#include "types/MembraneType.hpp"

#include <stdexcept>

namespace cell
{

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
DiscType::DiscType(const std::string& name, const sf::Color& color, double radius, double mass)
{
    setName(name);
    setColor(color);
    setRadius(radius);
    setMass(mass);
}

double DiscType::getRadius() const
{
    return radius_;
}

void DiscType::setRadius(double radius)
{
    if (radius <= 0)
        throw ExceptionWithLocation("Disc type radius must be positive");

    radius_ = radius;
}

double DiscType::getMass() const
{
    return mass_;
}

void DiscType::setMass(double mass)
{
    if (mass <= 0)
        throw ExceptionWithLocation("Disc type mass must be positive");

    mass_ = mass;
}

} // namespace cell