#include "Particle.hpp"

Particle::Particle(float radius, std::size_t pointCount) : sf::CircleShape(radius, pointCount), mass(radius)
{
    setOrigin({radius, radius});
}

void Particle::update(const sf::Time& dt)
{
    move(dt.asSeconds() * velocity);
}

sf::Vector2f Particle::getPosition() const
{
    return sf::Transformable::getPosition();
}
