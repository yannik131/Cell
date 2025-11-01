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
    simulationContext_.collisionHandler.calculateDiscMembraneCollisionResponse(collisions.discChildMembraneCollisions);
    moveDiscsIntoChildCompartments(collisions.discContainingMembraneCollisions);
    simulationContext_.reactionEngine.applyBimolecularReactions(collisions.discDiscCollisions);
    simulationContext_.collisionHandler.calculateDiscDiscCollisionResponse(collisions.discDiscCollisions);

    // TODO: Calculate both TOIs for simultaneous bounds/discs collisions and only handle the earlier one
    moveDiscsIntoParentCompartment();
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

void Compartment::moveDiscsIntoChildCompartments(auto& discMembraneCollisions)
{
    for (auto& [disc, membrane] : discMembraneCollisions)
    {
        const auto& discRadius = simulationContext_.discTypeRegistry.getByID(disc->getTypeID()).getRadius();
        const auto& membraneRadius = simulationContext_.membraneTypeRegistry.getByID(membrane->getTypeID()).getRadius();
        if (mathutils::circleIsFullyContainedByCircle(disc->getPosition(), discRadius, membrane->getPosition(),
                                                      membraneRadius))
        {
            membrane->getCompartment()->addDisc(*disc);
            disc->markDestroyed();
        }
        else
            membrane->getCompartment()->addIntrudingDisc(*disc);
    }
}

void Compartment::moveDiscsIntoParentCompartment()
{
    const auto& membraneType = simulationContext_.membraneTypeRegistry.getByID(membrane_.getTypeID());
    const auto M = membrane_.getPosition();
    const auto R = membraneType.getRadius();

    for (std::size_t i = 0; i < discs_.size(); ++i)
    {
        auto& disc = discs_[i];
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
}

void Compartment::updateChildCompartments(double dt)
{
    for (auto& compartment : compartments_)
        compartment->update(dt);
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