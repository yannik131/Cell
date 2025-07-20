#include "TypeBase.hpp"
#include "ExceptionWithLocation.hpp"

namespace cell
{

const std::string& TypeBase::getName() const
{
    return name_;
}

void TypeBase::setName(const std::string& name)
{
    if (name.empty())
        throw ExceptionWithLocation("Disc type name cannot be empty");

    name_ = name;
}

const sf::Color& TypeBase::getColor() const
{
    return color_;
}

void TypeBase::setColor(const sf::Color& color)
{
    if (color == sf::Color())
        throw ExceptionWithLocation("Disc type must have a valid color");

    color_ = color;
}

}