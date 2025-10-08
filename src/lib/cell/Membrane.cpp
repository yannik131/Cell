#include "Membrane.hpp"

namespace cell
{

Membrane::Membrane(const MembraneTypeID& membraneTypeID)
    : membraneTypeID_(membraneTypeID)
{
}

const sf::Vector2d& Membrane::getPosition() const
{
    return position_;
}

void Membrane::setPosition(const sf::Vector2d& position)
{
    position_ = position;
}

MembraneTypeID Membrane::getTypeID() const
{
    return membraneTypeID_;
}

void Membrane::setCompartment(Compartment* compartment)
{
    compartment_ = compartment;
}

Compartment* Membrane::getCompartment() const
{
    return compartment_;
}

} // namespace cell