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

} // namespace cell