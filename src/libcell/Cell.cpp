#include "Cell.hpp"
#include "MathUtils.hpp"
#include "NanoflannAdapter.hpp"
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

Cell::Cell(const ReactionEngine& reactionEngine)
    : reactionEngine_(&reactionEngine)
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

    for (auto& disc : discs_)
    {
        disc.move(disc.getVelocity() * static_cast<double>(dt.asSeconds()));
        state_->currentKineticEnergy_ +=
            mathutils::handleWorldBoundCollision(disc, {0, 0}, {state_->cellWidth_, state_->cellHeight_},
                                                 state_->initialKineticEnergy_ - state_->currentKineticEnergy_);
    }

    const auto& newDiscs = reactionEngine_->unimolecularReactions(discs_);
    newDiscs_.insert(newDiscs_.end(), newDiscs.begin(), newDiscs.end());
    discs_.insert(discs_.end(), newDiscs_.begin(), newDiscs_.end());

    const auto& collidingDiscs = mathutils::findCollidingDiscs(discs_, state_->maxRadius_);
    collisionCounts_ += mathutils::handleDiscCollisions(collidingDiscs);

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
    return discs_;
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
    for (auto iter = discs_.begin(); iter != discs_.end();)
    {
        if (iter->isMarkedDestroyed())
            iter = discs_.erase(iter);
        else
        {
            state_->currentKineticEnergy_ += iter->getKineticEnergy(state_->discTypeResolver_);
            ++iter;
        }
    }
}

} // namespace cell