#include "Cell.hpp"
#include "CollisionDetector.hpp"
#include "CollisionHandler.hpp"
#include "Disc.hpp"
#include "MathUtils.hpp"
#include "ReactionEngine.hpp"
#include "Reactions.hpp"

#include <glog/logging.h>

#include <algorithm>
#include <cmath>
#include <execution>
#include <map>
#include <random>
#include <set>

namespace cell
{
Cell::Cell(const ReactionEngine* reactionEngine, const CollisionDetector* collisionDetector,
           const CollisionHandler* collisionHandler)
    : reactionEngine_(reactionEngine)
    , collisionDetector_(collisionDetector)
    , collisionHandler_(collisionHandler)
{
}

void Cell::setState(CellState&& state)
{
    state_ = std::make_unique<CellState>(std::move(state));
}

template <typename T> DiscTypeMap<T> operator+=(DiscTypeMap<T>& a, const DiscTypeMap<T>& b)
{
    for (const auto& [key, value] : b)
        a[key] += value;

    return a;
}

void Cell::update(const sf::Time& dt)
{
    newDiscs_.clear();

    const sf::Vector2d topLeft{0, 0};
    const sf::Vector2d bottomRight{state_->cellWidth_, state_->cellHeight_};

    for (auto& disc : state_->discs_)
    {
        disc.move(disc.getVelocity() * static_cast<double>(dt.asSeconds()));

        auto collision = collisionDetector_->detectDiscRectangleCollision(disc, topLeft, bottomRight);
        collisionHandler_->calculateDiscRectangleCollisionResponse(disc, collision);

        state_->currentKineticEnergy_ += collisionHandler_->keepKineticEnergyConstant(
            disc, collision, state_->initialKineticEnergy_ - state_->currentKineticEnergy_);
    }

    const auto& newDiscs = reactionEngine_->unimolecularReactions(state_->discs_);
    newDiscs_.insert(newDiscs_.end(), newDiscs.begin(), newDiscs.end());
    state_->discs_.insert(state_->discs_.end(), newDiscs_.begin(), newDiscs_.end());

    auto collidingDiscs = collisionDetector_->detectDiscDiscCollisions(state_->discs_);
    collisionCounts_ += collisionHandler_->calculateDiscDiscCollisionResponse(collidingDiscs);

    removeDestroyedDiscs();
}

DiscTypeMap<int> Cell::getAndResetCollisionCount()
{
    auto tmp = std::move(collisionCounts_);
    collisionCounts_.clear();

    return tmp;
}

const std::vector<Disc>& Cell::getDiscs() const
{
    return state_->discs_;
}

double Cell::getInitialKineticEnergy() const
{
    return state_->initialKineticEnergy_;
}

double Cell::getCurrentKineticEnergy() const
{
    return state_->currentKineticEnergy_;
}

void Cell::removeDestroyedDiscs()
{
    state_->currentKineticEnergy_ = 0.0;
    for (auto iter = state_->discs_.begin(); iter != state_->discs_.end();)
    {
        if (iter->isMarkedDestroyed())
            iter = state_->discs_.erase(iter);
        else
        {
            state_->currentKineticEnergy_ += iter->getKineticEnergy(state_->discTypeResolver_);
            ++iter;
        }
    }
}

} // namespace cell