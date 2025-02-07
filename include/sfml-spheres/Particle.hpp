#ifndef PARTICLE_H
#define PARTICLE_H

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>

class Particle : public sf::CircleShape {
public:
    Particle(float radius, std::size_t pointCount = 30);

    void update(const sf::Time& dt);
    
    sf::Vector2f getPosition() const;
    
public:
    sf::Vector2f velocity;
    float mass;
};

#endif /* PARTICLE_H */
