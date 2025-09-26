#include "Cell.hpp"
#include "CollisionDetector.hpp"
#include "CollisionHandler.hpp"
#include "Disc.hpp"
#include "MathUtils.hpp"
#include "ReactionEngine.hpp"
#include "Settings.hpp"

namespace cell
{

Cell::Cell(ReactionEngine& reactionEngine, CollisionDetector& collisionDetector, CollisionHandler& collisionHandler,
           const DiscTypeRegistry& discTypeRegistry, Dimensions dimensions, std::vector<Disc>&& discs)
    : reactionEngine_(reactionEngine)
    , collisionDetector_(collisionDetector)
    , collisionHandler_(collisionHandler)
    , discTypeRegistry_(discTypeRegistry)
    , width_(dimensions.width)
    , height_(dimensions.height)
    , discs_(std::move(discs))
{
    throwIfNotInRange(width_, SettingsLimits::MinCellWidth, SettingsLimits::MaxCellWidth, "cell width");
    throwIfNotInRange(height_, SettingsLimits::MinCellHeight, SettingsLimits::MaxCellHeight, "cell height");

    for (const auto& disc : discs_)
        initialKineticEnergy_ += disc.getKineticEnergy(discTypeRegistry_.getByID(disc.getDiscTypeID()));
}

void Cell::update()
{
    const double dt = simulationTimeStepProvider_();
    const sf::Vector2d topLeft{0, 0};
    const sf::Vector2d bottomRight{static_cast<double>(width_), static_cast<double>(height_)};
    std::vector<Disc> newDiscs;

    for (auto& disc : discs_)
    {
        // A -> B returns nothing, A -> B + C returns 1 new disc
        if (auto newDisc = reactionEngine_.applyUnimolecularReactions(disc))
            newDiscs.push_back(std::move(*newDisc));

        disc.move(disc.getVelocity() * dt);

        auto collision = collisionDetector_.detectDiscRectangleCollision(disc, topLeft, bottomRight);
        collisionHandler_.calculateDiscRectangleCollisionResponse(disc, collision);

        // combination reactions are inelastic and consume energy
        currentKineticEnergy_ +=
            collisionHandler_.keepKineticEnergyConstant(disc, collision, initialKineticEnergy_ - currentKineticEnergy_);
    }

    discs_.insert(discs_.begin(), newDiscs.begin(), newDiscs.end());

    auto collidingDiscs = collisionDetector_.detectDiscDiscCollisions(discs_);

    // marks consumed discs as destroyed
    reactionEngine_.applyBimolecularReactions(collidingDiscs);

    // ignores marked discs
    collisionHandler_.calculateDiscDiscCollisionResponse(collidingDiscs);

    removeDestroyedDiscs();
}

const std::vector<Disc>& Cell::getDiscs() const
{
    return discs_;
}

double Cell::getInitialKineticEnergy() const
{
    return initialKineticEnergy_;
}

double Cell::getCurrentKineticEnergy() const
{
    return currentKineticEnergy_;
}

void Cell::removeDestroyedDiscs()
{
    currentKineticEnergy_ = 0.0;
    for (auto iter = discs_.begin(); iter != discs_.end();)
    {
        if (iter->isMarkedDestroyed())
            iter = discs_.erase(iter);
        else
        {
            currentKineticEnergy_ += iter->getKineticEnergy(discTypeResolver_);
            ++iter;
        }
    }
}

} // namespace cell