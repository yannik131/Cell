#ifndef WORLD_HPP
#define WORLD_HPP

#include "Disc.hpp"
#include "NanoflannAdapter.hpp"

#include <SFML/System/Time.hpp>

#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

class World
{
public:
    World();

    void update(const sf::Time& dt);
    DiscType::map<int> getAndResetCollisionCount();
    const std::vector<Disc>& discs() const;
    void reinitialize(); // Has to be called before update()!
    void setBounds(const sf::Vector2f& bounds);

private:
    void buildScene();
    void initializeStartPositions();
    void removeDestroyedDiscs();

private:
    std::vector<sf::Vector2f> startPositions_;
    std::vector<Disc> discs_;
    sf::Vector2f bounds_;
    int maxRadius_;
    DiscType::map<int> collisionCounts_;
    float initialKineticEnergy_;
    float currentKineticEnergy_;

    std::vector<int> destroyedDiscsIndices_;
    std::vector<Disc> newDiscs_;
};

#endif /* WORLD_HPP */
