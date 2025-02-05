#ifndef PARTICLE_H
#define PARTICLE_H

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/System/Time.hpp>

class Particle : public sf::CircleShape {
public:
    Particle(float radius, std::size_t pointCount = 30);
    
    void update(const sf::Time& dt);
    
    bool operator<(const Particle& other) const;

public:
    sf::Vector2f velocity;
};

#endif /* PARTICLE_H */
