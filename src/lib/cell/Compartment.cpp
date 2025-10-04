#include "Compartment.hpp"
#include "CollisionDetector.hpp"
#include "CollisionHandler.hpp"
#include "Disc.hpp"
#include "MathUtils.hpp"
#include "ReactionEngine.hpp"

namespace cell
{

Compartment::Compartment(Compartment* parent, Membrane membrane, SimulationContext simulationContext)
    : parent_(parent)
    , membrane_(std::move(membrane))
    , simulationContext_(std::move(simulationContext))
{
}

Compartment::~Compartment() = default;

const Membrane& Compartment::getMembrane() const
{
    return membrane_;
}

void Compartment::setDiscs(std::vector<Disc>&& discs)
{
    discs_ = std::move(discs);
}

void Compartment::addDisc(Disc&& disc)
{
    discs_.push_back(std::move(disc));
}

const std::vector<Disc>& Compartment::getDiscs() const
{
    return discs_;
}

std::vector<std::unique_ptr<Compartment>>& Compartment::getCompartments()
{
    return compartments_;
}

const std::vector<std::unique_ptr<Compartment>>& Compartment::getCompartments() const
{
    return compartments_;
}

const Compartment* Compartment::getParent() const
{
    return parent_;
}

void Compartment::update(double dt)
{
    std::vector<Disc> newDiscs;
    const auto M = membrane_.getPosition();
    const auto R = simulationContext_.membraneTypeRegistry.getByID(membrane_.getMembraneTypeID()).getRadius();

    for (auto& disc : discs_)
    {
        // A -> B returns nothing, A -> B + C returns 1 new disc
        if (auto newDisc = simulationContext_.reactionEngine.applyUnimolecularReactions(disc, dt))
            newDiscs.push_back(std::move(*newDisc));

        disc.move(disc.getVelocity() * dt);

        if (simulationContext_.collisionDetector.detectCircularBoundsCollision(disc, M, R))
            simulationContext_.collisionHandler.calculateCircularBoundsCollisionResponse(disc, M, R);
    }

    discs_.insert(discs_.begin(), newDiscs.begin(), newDiscs.end());

    auto collidingDiscs = simulationContext_.collisionDetector.detectDiscDiscCollisions(discs_);

    // marks consumed discs as destroyed
    simulationContext_.reactionEngine.applyBimolecularReactions(collidingDiscs);

    // ignores marked discs
    simulationContext_.collisionHandler.calculateDiscDiscCollisionResponse(collidingDiscs);

    removeDestroyedDiscs();

    for (auto& compartment : compartments_)
        compartment->update(dt);
}

Compartment* Compartment::createSubCompartment(Membrane membrane)
{
    compartments_.push_back(std::make_unique<Compartment>(this, std::move(membrane), simulationContext_));

    return compartments_.back().get();
}

void Compartment::removeDestroyedDiscs()
{
    for (std::size_t i = 0; i < discs_.size();)
    {
        if (discs_[i].isMarkedDestroyed())
        {
            discs_[i] = std::move(discs_.back());
            discs_.pop_back();
        }
        else
            ++i;
    }
}

} // namespace cell