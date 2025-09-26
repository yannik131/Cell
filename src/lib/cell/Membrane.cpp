#include "Membrane.hpp"

namespace cell
{

Membrane::Membrane(const sf::Vector2d& position)
    : position_(position)
{
}

const sf::Vector2d& Membrane::getPosition() const
{
    return position_;
}

} // namespace cell