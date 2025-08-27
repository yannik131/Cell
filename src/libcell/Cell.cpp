#include "Cell.hpp"
#include "CollisionDetector.hpp"
#include "CollisionHandler.hpp"
#include "Disc.hpp"
#include "MathUtils.hpp"
#include "ReactionEngine.hpp"

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

void Cell::update(const sf::Time& dt)
{
    const sf::Vector2d topLeft{0, 0};
    const sf::Vector2d bottomRight{static_cast<double>(state_->cellWidth_), static_cast<double>(state_->cellHeight_)};
    static std::vector<Disc> newDiscs;
    newDiscs.clear();

    for (auto& disc : state_->discs_)
    {
        auto product = reactionEngine_->applyUnimolecularReactions(disc);
        if (product)
            newDiscs.push_back(std::move(*product));

        disc.move(disc.getVelocity() * static_cast<double>(dt.asSeconds()));

        auto collision = collisionDetector_->detectDiscRectangleCollision(disc, topLeft, bottomRight);
        collisionHandler_->calculateDiscRectangleCollisionResponse(disc, collision);

        state_->currentKineticEnergy_ += collisionHandler_->keepKineticEnergyConstant(
            disc, collision, state_->initialKineticEnergy_ - state_->currentKineticEnergy_);
    }

    // iterate over discs again here
    // 1. check for collision, continue if not
    // 2. attempt bimolecular reactions
    // continue if reaction happened, mark educts as consumed
    // 3. calculate collision response with remaining colliding discs
    // 4. after loop, add products to discs and remove educts

    const auto& newDiscs = reactionEngine_->applyUnimolecularReactions(state_->discs_);
    state_->discs_.insert(state_->discs_.end(), newDiscs.begin(), newDiscs.end());

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