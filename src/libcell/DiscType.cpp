#include "DiscType.hpp"

bool operator==(const DiscType& a, const DiscType& b)
{
    return a.name_ == b.name_;
}

bool operator<(const DiscType& a, const DiscType& b)
{
    return a.name_ < b.name_;
}

DiscType::DiscType(const std::string& name, const sf::Color& color, float radius, float mass)
    : name_(name)
    , color_(color)
    , radius_(radius)
    , mass_(mass)
{
}
