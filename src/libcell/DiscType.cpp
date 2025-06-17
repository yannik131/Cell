#include "DiscType.hpp"
#include "ExceptionWithLocation.hpp"
#include "MathUtils.hpp"

#include <stdexcept>

namespace cell
{

int DiscType::instanceCount = 0;

std::pair<DiscType, DiscType> makeOrderedPair(const DiscType& d1, const DiscType& d2)
{
    return d2.getId() < d1.getId() ? std::make_pair(d2, d1) : std::make_pair(d1, d2);
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
DiscType::DiscType(const std::string& name, const sf::Color& color, float radius, float mass)
    : id_(instanceCount++)
{
    setName(name);
    setColor(color);
    setRadius(radius);
    setMass(mass);
}

DiscType::DiscType(const DiscType& other) = default;
DiscType& DiscType::operator=(const DiscType& other) = default;

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

const sf::Color& DiscType::getColor() const
{
    return color_;
}

void DiscType::setColor(const sf::Color& color)
{
    if (color == sf::Color())
        throw ExceptionWithLocation("Disc type must have a valid color");

    color_ = color;
}

float DiscType::getRadius() const
{
    return radius_;
}

void DiscType::setRadius(float radius)
{
    if (radius <= 0)
        throw ExceptionWithLocation("Disc type radius must be positive");

    radius_ = radius;
}

float DiscType::getMass() const
{
    return mass_;
}

void DiscType::setMass(float mass)
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