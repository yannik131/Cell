#include "DiscType.hpp"

#include <stdexcept>

bool operator==(const DiscType& a, const DiscType& b)
{
    return a.name_ == b.name_;
}

bool operator<(const DiscType& a, const DiscType& b)
{
    return a.name_ < b.name_;
}

std::pair<DiscType, DiscType> makeOrderedPair(const DiscType& d1, const DiscType& d2)
{
    return d2 < d1 ? std::make_pair(d2, d1) : std::make_pair(d1, d2);
}

bool isValid(const DiscType& discType)
{
    return !discType.name_.empty() && discType.color_ != sf::Color() && discType.mass_ >= 0 && discType.radius_ >= 0;
}

DiscType::DiscType(const std::string& name, const sf::Color& color, float radius, float mass)
    : name_(name)
    , color_(color)
    , radius_(radius)
    , mass_(mass)
{
}
