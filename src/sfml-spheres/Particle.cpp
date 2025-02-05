#include "Particle.hpp"

Particle::Particle(float radius, std::size_t pointCount) : sf::CircleShape(radius, pointCount)
{
}

void Particle::update(const sf::Time& dt)
{
    move(dt.asSeconds() * velocity);
}

bool Particle::operator<(const Particle& other) const
{
    if (getPosition().x != other.getPosition().x) {
        return getPosition().x < other.getPosition().x; // Compare x first
    }
    return getPosition().y < other.getPosition().y; // If x is equal, compare y
}
