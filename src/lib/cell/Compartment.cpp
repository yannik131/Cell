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

auto Compartment::detectCollisions()
{
    simulationContext_.collisionDetector.buildEntries(discs_, membranes_, intrudingDiscs_);
    return simulationContext_.collisionDetector.detectCollisions(
        CollisionDetector::Params{.discs = &discs_,
                                  .membranes = &membranes_,
                                  .intrudingDiscs = &intrudingDiscs_,
                                  .containingMembrane = &membrane_});
}

void Compartment::update(double dt)
{
    moveDiscsAndApplyUnimolecularReactions(dt);

    auto collisions = detectCollisions();

    simulationContext_.collisionHandler.calculateDiscChildMembraneCollisionResponse(
        collisions.discChildMembraneCollisions);
    moveDiscsIntoChildCompartments(collisions.discChildMembraneCollisions);

    simulationContext_.reactionEngine.applyBimolecularReactions(collisions.discDiscCollisions);
    simulationContext_.collisionHandler.calculateDiscDiscCollisionResponse(collisions.discDiscCollisions);

    simulationContext_.collisionHandler.calculateDiscContainingMembraneCollisionResponse(
        collisions.discContainingMembraneCollisions);
    moveDiscsIntoParentCompartment(collisions.discContainingMembraneCollisions);

    updateChildCompartments(dt);

    removeDestroyedDiscs();
    intrudingDiscs_.clear();
}

Compartment* Compartment::createSubCompartment(Membrane membrane)
{
    auto compartment = std::make_unique<Compartment>(this, std::move(membrane), simulationContext_);
    membranes_.push_back(compartment->getMembrane());
    compartments_.push_back(std::move(compartment));

    return compartments_.back().get();
}

void Compartment::moveDiscsAndApplyUnimolecularReactions(double dt)
{
    std::vector<Disc> newDiscs;

    for (auto& disc : discs_)
    {
        // A -> B returns nothing, A -> B + C returns 1 new disc
        if (auto newDisc = simulationContext_.reactionEngine.applyUnimolecularReactions(disc, dt))
        {
            newDisc->move(newDisc->getVelocity() * dt);
            newDiscs.push_back(std::move(*newDisc));
        }

        disc.move(disc.getVelocity() * dt);
    }

    if (!newDiscs.empty())
        discs_.insert(discs_.end(), newDiscs.begin(), newDiscs.end());
}

void Compartment::moveDiscsIntoChildCompartments(auto& discChildMembraneCollisions)
{
    for (auto& collision : discChildMembraneCollisions)
    {
        const auto& discRadius = simulationContext_.discTypeRegistry.getByID(collision.disc->getTypeID()).getRadius();
        const auto& membraneRadius =
            simulationContext_.membraneTypeRegistry.getByID(collision.membrane->getTypeID()).getRadius();
        if (mathutils::circleIsFullyContainedByCircle(collision.disc->getPosition(), discRadius,
                                                      collision.membrane->getPosition(), membraneRadius))
        {
            collision.membrane->getCompartment()->addDisc(*collision.disc);
            collision.disc->markDestroyed();
        }
        else
            collision.membrane->getCompartment()->addIntrudingDisc(*collision.disc);
    }
}

void Compartment::moveDiscsIntoParentCompartment(auto& discContainingMembraneCollisions)
{
    const auto& membraneType = simulationContext_.membraneTypeRegistry.getByID(membrane_.getTypeID());
    const auto M = membrane_.getPosition();
    const auto R = membraneType.getRadius();

    for (auto& collision : discContainingMembraneCollisions)
    {
        // Note: parent is nullptr for the containing cell, so it should never have outward permeability
        const auto discRadius = simulationContext_.discTypeRegistry.getByID(collision.disc->getTypeID()).getRadius();
        if (!mathutils::circlesOverlap(collision.disc->getPosition(), discRadius, M, R))
        {
            parent_->addDisc(*collision.disc);
            collision.disc->markDestroyed();
        }
        else
            parent_->addIntrudingDisc(*collision.disc);
    }
}

void Compartment::updateChildCompartments(double dt)
{
    for (auto& compartment : compartments_)
        compartment->update(dt);
}

void Compartment::removeDestroyedDiscs()
{
    discs_.erase(
        std::remove_if(discs_.begin(), discs_.end(), [](const Disc& disc) { return disc.isMarkedDestroyed(); }),
        discs_.end());
}

} // namespace cell