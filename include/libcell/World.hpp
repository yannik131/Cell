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

    /**
     * @brief Returns indices of destroyed discs in ascending order
     */
    const std::vector<int>& getDestroyedDiscsIndices() const;
    const std::vector<int>& getChangedDiscsIndices() const;
    const std::vector<Disc>& getNewDiscs() const;

private:
    void buildScene();
    void initializeStartPositions();
    void findChangedDiscs();
    void removeDestroyedDiscs();
    void handleDecompositionReactions();

private:
    std::vector<sf::Vector2f> startPositions_;
    std::vector<Disc> discs_;
    sf::Vector2f bounds_;
    int maxRadius_;
    int collisionCount_ = 0;

    std::vector<int> changedDiscsIndices_;
    std::vector<int> destroyedDiscsIndices_;
    std::vector<Disc> newDiscs_;
};

#endif /* WORLD_HPP */
