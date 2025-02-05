#ifndef WORLD_HPP
#define WORLD_HPP

#include "Particle.hpp"

#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include <vector>
#include <map>
#include <array>

class World : private sf::NonCopyable {
public:
    World(sf::RenderWindow& renderWindow);
    
    void update(const sf::Time& dt);
    void draw();
    
private:
    void buildScene();
    void initializeStartPositions();
    void handleWorldBoundCollision(Particle& particle);
    void handleParticleCollisions();
    
private:
    const std::map<float, int> RadiusDistribution_ = {{0.1, 5}, {0.2, 10}, {0.3, 12}, {0.4, 15}};
    const std::map<int, sf::Color> Colors_ = {{5, sf::Color::Green}, {10, sf::Color::Red}, {12, sf::Color::Blue}, {15, sf::Color::Yellow}};
    const int ParticleCount = 200;
    std::vector<sf::Vector2f> startPositions_;
    std::vector<Particle> particles_;
    sf::RenderWindow& renderWindow_;
};

#endif /* WORLD_HPP */
