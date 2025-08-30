#include "Cell.hpp"
#include "CollisionDetector.hpp"
#include "CollisionHandler.hpp"
#include "Disc.hpp"
#include "MathUtils.hpp"
#include "ReactionEngine.hpp"

namespace cell
{
Cell::Cell(ReactionEngine& reactionEngine, CollisionDetector& collisionDetector, CollisionHandler& collisionHandler,
           SimulationTimeStepProvider simulationTimeStepProvider)
    : reactionEngine_(reactionEngine)
    , collisionDetector_(collisionDetector)
    , collisionHandler_(collisionHandler)
    , simulationTimeStepProvider_(std::move(simulationTimeStepProvider))
{
}

void Cell::setState(CellState&& state)
{
    state_ = std::make_unique<CellState>(std::move(state));
}

void Cell::update()
{
    const double dt = simulationTimeStepProvider_();
    const sf::Vector2d topLeft{0, 0};
    const sf::Vector2d bottomRight{static_cast<double>(state_->cellWidth_), static_cast<double>(state_->cellHeight_)};
    static std::vector<Disc> newDiscs;
    newDiscs.clear();

    for (auto& disc : state_->discs_)
    {
        if (auto newDisc = reactionEngine_.applyUnimolecularReactions(disc))
            newDiscs.push_back(std::move(*newDisc));

        disc.move(disc.getVelocity() * dt);

        auto collision = collisionDetector_.detectDiscRectangleCollision(disc, topLeft, bottomRight);
        collisionHandler_.calculateDiscRectangleCollisionResponse(disc, collision);

        // combination reactions are inelastic and consume energy
        state_->currentKineticEnergy_ += collisionHandler_.keepKineticEnergyConstant(
            disc, collision, state_->initialKineticEnergy_ - state_->currentKineticEnergy_);
    }

    state_->discs_.insert(state_->discs_.begin(), newDiscs.begin(), newDiscs.end());

    auto collidingDiscs = collisionDetector_.detectDiscDiscCollisions(state_->discs_);

    // marks consumed discs as destroyed
    reactionEngine_.applyBimolecularReactions(collidingDiscs);

    // ignores marked discs
    collisionHandler_.calculateDiscDiscCollisionResponse(collidingDiscs);

    removeDestroyedDiscs();
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