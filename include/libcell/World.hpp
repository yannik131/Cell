#ifndef WORLD_HPP
#define WORLD_HPP

#include "Disc.hpp"
#include "NanoflannAdapter.hpp"

#include <SFML/System/Time.hpp>

#include <map>
#include <memory>
#include <set>
#include <vector>

class World
{
public:
    World();

    void update(const sf::Time& dt);
    int getAndResetCollisionCount();
    const std::vector<Disc>& discs() const;
    void reinitialize(); // Has to be called before update()!
    void setBounds(const sf::Vector2f& bounds);

private:
    void buildScene();
    void initializeStartPositions();

private:
    std::vector<sf::Vector2f> startPositions_;
    std::vector<Disc> discs_;
    sf::Vector2f bounds_;
    int maxRadius_;
    int collisionCount_ = 0;
};

#endif /* WORLD_HPP */
