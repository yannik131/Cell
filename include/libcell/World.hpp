#ifndef WORLD_HPP
#define WORLD_HPP

#include "Disc.hpp"

#include <SFML/System/Time.hpp>

#include <vector>
#include <map>
#include <set>

class World {
public:
    World();
    
    void update(const sf::Time& dt);
    int getAndResetCollisionCount();
    const std::vector<Disc>& discs() const;
    void reset(); //Has to be called before update()!
    void setNumberOfDiscs(int numberOfDiscs);
    void setBounds(const sf::Vector2f& bounds);
    
private:
    void buildScene();
    void initializeStartPositions();
    std::set<std::pair<Disc*, Disc*>> findCollidingDiscs();
    void handleWorldBoundCollision(Disc& disc);
    void handleDiscCollisions(const std::set<std::pair<Disc*, Disc*>>& collidingDiscs, const sf::Time& dt);
    
private:
    const std::map<float, int> RadiusDistribution_ = {{0.5, 5}, {0.7, 10}, {0.9, 12}, {1, 15}};
    int numberOfDiscs_ = 50;
    std::vector<sf::Vector2f> startPositions_;
    std::vector<Disc> discs_;
    sf::Vector2f bounds_;
    int maxRadius_;
    int collisionCount_ = 0;
};

#endif /* WORLD_HPP */
