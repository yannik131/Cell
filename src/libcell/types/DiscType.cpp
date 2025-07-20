#include "DiscType.hpp"
#include "ExceptionWithLocation.hpp"
#include "MathUtils.hpp"
#include "MembraneType.hpp"

#include <stdexcept>

namespace cell
{

int DiscType::instanceCount = 0;

std::pair<DiscType, DiscType> makeOrderedPair(const DiscType& d1, const DiscType& d2)
{
    return d2.getId() < d1.getId() ? std::make_pair(d2, d1) : std::make_pair(d1, d2);
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
DiscType::DiscType(const std::string& name, const sf::Color& color, double radius, double mass)
    : id_(instanceCount++)
{
    setName(name);
    setColor(color);
    setRadius(radius);
    setMass(mass);
}

DiscType::DiscType(const DiscType& other) = default;
DiscType& DiscType::operator=(const DiscType& other) = default;

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

int DiscType::getId() const
{
    return id_;
}

bool DiscType::operator==(const DiscType& other) const
{
    return name_ == other.name_ && color_ == other.color_ && radius_ == other.radius_ && mass_ == other.mass_ &&
           id_ == other.id_;
}

int DiscType::IdHasher::operator()(const DiscType& discType) const
{
    return discType.getId();
}

int DiscType::PairHasher::operator()(const std::pair<DiscType, DiscType>& pair) const
{
    return mathutils::calculateHash(pair.first.getId(), pair.second.getId());
}

} // namespace cell