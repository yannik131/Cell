#include "DiscType.hpp"
#include "ExceptionWithLocation.hpp"
#include "MathUtils.hpp"

namespace cell
{

DiscType::DiscType(const std::string& name, Radius radius, Mass mass)
{
    setName(name);
    setRadius(radius.value);
    setMass(mass.value);
}

const std::string& DiscType::getName() const
{
    return name_;
}

void DiscType::setName(const std::string& name)
{
    if (name.empty())
        throw ExceptionWithLocation("Disc type name cannot be empty");

    name_ = name;
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

bool DiscType::operator==(const DiscType& other) const
{
    return name_ == other.name_ && radius_ == other.radius_ && mass_ == other.mass_;
}

} // namespace cell