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
    membrane_.setCompartment(this);
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

void Compartment::addDisc(Disc disc)
{
    discs_.push_back(std::move(disc));
}

const std::vector<Disc>& Compartment::getDiscs() const
{
    return discs_;
}

void Compartment::addIntrudingDisc(Disc& disc)
{
    intrudingDiscs_.push_back(&disc);
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
    const auto& membraneType = simulationContext_.membraneTypeRegistry.getByID(membrane_.getTypeID());
    const auto M = membrane_.getPosition();
    const auto R = membraneType.getRadius();
    std::vector<Disc> newDiscs;

    for (std::size_t i = 0; i < discs_.size(); ++i)
    {
        auto& disc = discs_[i];

        // A -> B returns nothing, A -> B + C returns 1 new disc
        if (auto newDisc = simulationContext_.reactionEngine.applyUnimolecularReactions(disc, dt))
            newDiscs.push_back(std::move(*newDisc));

        disc.move(disc.getVelocity() * dt);

        if (!simulationContext_.collisionDetector.detectCircularBoundsCollision(disc, M, R))
            continue;

        if (static_cast<int>(membraneType.getPermeabilityFor(disc.getTypeID())) &
            (static_cast<int>(MembraneType::Permeability::None) | static_cast<int>(MembraneType::Permeability::Inward)))
        {
            simulationContext_.collisionHandler.calculateCircularBoundsCollisionResponse(disc, M, R);
            continue;
        }

        // Note: parent is nullptr for the containing cell, so it should never have outward permeability
        const auto& discRadius = simulationContext_.discTypeRegistry.getByID(disc.getTypeID()).getRadius();
        if (!mathutils::circlesOverlap(disc.getPosition(), discRadius, M, R))
        {
            std::swap(discs_[i], discs_.back());
            parent_->addDisc(std::move(disc));
            discs_.pop_back();
            --i;
        }
        else
            parent_->addIntrudingDisc(disc);
    }

    discs_.insert(discs_.begin(), newDiscs.begin(), newDiscs.end());

    // Handle collisions with intruding discs after moving the discs in this compartment to avoid overlap after moving
    simulationContext_.collisionDetector.buildEntries(discs_, membranes_, intrudingDiscs_);
    auto collisions = simulationContext_.collisionDetector.detectCollisions(&discs_, &membranes_, &intrudingDiscs_);

    // TODO when inserting into child compartment, directly insert into newDiscs to avoid double update

    simulationContext_.collisionHandler.calculateMembraneDiscCollisionResponse(collisions.discMembraneCollisions);

    // marks consumed discs as destroyed
    simulationContext_.reactionEngine.applyBimolecularReactions(collisions.discDiscCollisions);

    removeDestroyedDiscs();

    simulationContext_.collisionHandler.calculateDiscDiscCollisionResponse(collisions.discDiscCollisions);

    for (auto& compartment : compartments_)
        compartment->update(dt);
}

Compartment* Compartment::createSubCompartment(Membrane membrane)
{
    auto compartment = std::make_unique<Compartment>(this, std::move(membrane), simulationContext_);
    membranes_.push_back(compartment->getMembrane());
    compartments_.push_back(std::move(compartment));

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

void Compartment::handleOuterMembraneCollision(Disc& disc)
{
}

} // namespace cell