#ifndef WORLD_HPP
#define WORLD_HPP

#include "Particle.hpp"

#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include <vector>
#include <map>
#include <set>

struct ParticlePairComparator
{
    bool operator()(const std::pair<Particle*, Particle*>& a, const std::pair<Particle*, Particle*>& b) const
    {
        auto normalizedA = (a.first < a.second) ? a : std::make_pair(a.second, a.first);
        auto normalizedB = (b.first < b.second) ? b : std::make_pair(b.second, b.first);

        return normalizedA < normalizedB;
    }
};

typedef std::set<std::pair<Particle*, Particle*>, ParticlePairComparator> CollisionSet;

class World : private sf::NonCopyable {
public:
    World(sf::RenderWindow& renderWindow);
    
    void update(const sf::Time& dt);
    void draw();
    
private:
    void buildScene();
    void initializeStartPositions();
    CollisionSet findCollidingParticles();
    void handleWorldBoundCollision(Particle& particle);
    
    /**
     * @brief Stolen from https://hermann-baum.de/bouncing-balls/
     */
    void handleParticleCollisions(const CollisionSet& collidingParticles, const sf::Time& dt);
    
private:
    const std::map<float, int> RadiusDistribution_ = {{0.5, 5}, {0.7, 10}, {0.9, 12}, {1, 15}};
    const std::map<int, sf::Color> Colors_ = {{5, sf::Color::Green}, {10, sf::Color::Red}, {12, sf::Color::Blue}, {15, sf::Color::Yellow}};
    const int ParticleCount = 250;
    std::vector<sf::Vector2f> startPositions_;
    std::vector<Particle> particles_;
    sf::RenderWindow& renderWindow_;
};

#endif /* WORLD_HPP */
