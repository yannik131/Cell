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
    int getAndResetCollisionCount();
    const std::vector<Disc>& discs() const;
    void reinitialize(); // Has to be called before update()!
    void setBounds(const sf::Vector2f& bounds);

private:
    void buildScene();
    void initializeStartPositions();
    void findChangedDiscs();
    void removeDestroyedDiscs();

private:
    std::vector<sf::Vector2f> startPositions_;
    std::vector<Disc> discs_;
    sf::Vector2f bounds_;
    int maxRadius_;
    int collisionCount_ = 0;
    float initialKineticEnergy_;
    float currentKineticEnergy_;

    // TODO Remove changed discs (we're emitting everything now)
    std::vector<int> changedDiscIndices_;
    std::vector<int> destroyedDiscsIndices_;
    std::vector<Disc> newDiscs_;
};

#endif /* WORLD_HPP */
