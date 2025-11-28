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
    auto detectedCollisions = detectCollisions();

    simulationContext_.reactionEngine.applyBimolecularReactions(detectedCollisions);
    moveDiscsIntoChildCompartments(detectedCollisions);
    moveDiscsIntoParentCompartment(detectedCollisions);
    simulationContext_.collisionHandler.resolveCollisions(detectedCollisions);
    updateChildCompartments(dt);
    moveDiscsAndApplyUnimolecularReactions(dt); // Removes destroyed discs
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

    for (std::size_t i = 0; i < discs_.size(); ++i)
    {
        auto& disc = discs_[i];
        if (disc.isMarkedDestroyed())
        {
            // This function also removes destroyed discs and thus needs to be called first
            discs_[i] = std::move(discs_.back());
            discs_.pop_back();
            --i;
            continue;
        }

        disc.move(disc.getVelocity() * dt);

        // A -> B returns nothing, A -> B + C returns 1 new disc
        if (auto newDisc = simulationContext_.reactionEngine.applyUnimolecularReactions(disc, dt))
            newDiscs.push_back(std::move(*newDisc));
    }

    if (!newDiscs.empty())
        discs_.insert(discs_.end(), newDiscs.begin(), newDiscs.end());
}

void Compartment::moveDiscsIntoChildCompartments(const CollisionDetector::DetectedCollisions& detectedCollisions)
{
    const auto& indexes = detectedCollisions.indexes.find(CollisionDetector::CollisionType::DiscChildMembrane);
    if (indexes == detectedCollisions.indexes.end())
        return;

    for (std::size_t index : indexes->second)
    {
        const auto& collision = detectedCollisions.collisions[index];
        if (collision.isInvalidatedByDestroyedDisc())
            continue;

        const auto permeability = simulationContext_.membraneTypeRegistry.getByID(collision.membrane->getTypeID())
                                      .getPermeabilityFor(collision.disc->getTypeID());
        if (permeability == MembraneType::Permeability::None || permeability == MembraneType::Permeability::Outward)
            continue;

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

void Compartment::moveDiscsIntoParentCompartment(const CollisionDetector::DetectedCollisions& detectedCollisions)
{
    // Only the case for the containing cell
    if (!parent_)
        return;

    const auto& indexes = detectedCollisions.indexes.find(CollisionDetector::CollisionType::DiscContainingMembrane);
    if (indexes == detectedCollisions.indexes.end())
        return;

    const auto& membraneType = simulationContext_.membraneTypeRegistry.getByID(membrane_.getTypeID());
    const auto M = membrane_.getPosition();
    const auto R = membraneType.getRadius();

    for (std::size_t index : indexes->second)
    {
        const auto& collision = detectedCollisions.collisions[index];
        if (collision.isInvalidatedByDestroyedDisc())
            continue;

        const auto permeability = simulationContext_.membraneTypeRegistry.getByID(collision.membrane->getTypeID())
                                      .getPermeabilityFor(collision.disc->getTypeID());
        if (permeability == MembraneType::Permeability::None || permeability == MembraneType::Permeability::Inward)
            continue;

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

} // namespace cell