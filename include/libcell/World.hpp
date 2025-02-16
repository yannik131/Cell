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
    std::set<std::pair<Disc*, Disc*>> findCollidingDiscs();
    void handleWorldBoundCollision(Disc& disc);
    void handleDiscCollisions(const std::set<std::pair<Disc*, Disc*>>& collidingDiscs, const sf::Time& dt);

private:
    std::vector<sf::Vector2f> startPositions_;
    std::vector<Disc> discs_;
    sf::Vector2f bounds_;
    int maxRadius_;
    int collisionCount_ = 0;

    // TODO put this stuff and the collision detection into own class
    typedef nanoflann::L2_Simple_Adaptor<float, NanoflannAdapter> AdapterType;
    typedef nanoflann::KDTreeSingleIndexAdaptor<AdapterType, NanoflannAdapter, 2> KDTree;
};

#endif /* WORLD_HPP */
