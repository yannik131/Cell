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
    auto collisions = simulationContext_.collisionDetector.detectCollisions(
        CollisionDetector::Params{.discs = &discs_,
                                  .membranes = &membranes_,
                                  .intrudingDiscs = &intrudingDiscs_,
                                  .containingMembrane = &membrane_});
    intrudingDiscs_.clear();
    return collisions;
}

void Compartment::update(double dt)
{
    auto detectedCollisions = detectCollisions();
    simulationContext_.reactionEngine.applyBimolecularReactions(detectedCollisions);
    simulationContext_.collisionHandler.resolveCollisions(detectedCollisions);

    updateChildCompartments(dt);

    moveDiscsAndApplyUnimolecularReactions(dt); // Removes destroyed discs
}

Compartment* Compartment::createSubCompartment(Membrane membrane)
{
    auto compartment = std::make_unique<Compartment>(this, std::move(membrane), simulationContext_);
    membranes_.push_back(compartment->getMembrane());
    compartments_.push_back(std::move(compartment));

    // Keep compartments sorted by left x to accomodate sweep & prune
    std::sort(compartments_.begin(), compartments_.end(),
              [&](const std::unique_ptr<Compartment>& lhs, const std::unique_ptr<Compartment>& rhs)
              {
                  const auto& lhsMembrane = lhs->getMembrane();
                  const auto& rhsMembrane = rhs->getMembrane();
                  const auto lhsLeftX =
                      lhsMembrane.getPosition().x -
                      simulationContext_.membraneTypeRegistry.getByID(lhsMembrane.getTypeID()).getRadius();
                  const auto rhsLeftX =
                      rhsMembrane.getPosition().x -
                      simulationContext_.membraneTypeRegistry.getByID(rhsMembrane.getTypeID()).getRadius();

                  return lhsLeftX < rhsLeftX;
              });

    compartmentEntries_.clear();
    for (const auto& compartment : compartments_)
    {
        const auto* membraneType =
            &simulationContext_.membraneTypeRegistry.getByID(compartment->getMembrane().getTypeID());
        double rightX = compartment->getMembrane().getPosition().x + membraneType->getRadius();
        compartmentEntries_.push_back(CompartmentEntry{.rightX = rightX,
                                                       .membraneType = membraneType,
                                                       .membrane = &compartment->getMembrane(),
                                                       .compartment = compartment.get()});
    }

    return compartments_.back().get();
}

void Compartment::moveDiscsAndApplyUnimolecularReactions(double dt)
{
    std::vector<Disc> newDiscs;

    const auto& membraneType = simulationContext_.membraneTypeRegistry.getByID(membrane_.getTypeID());
    const auto M = membrane_.getPosition();
    const auto R = membraneType.getRadius();

    const auto swapAndPop = [&](std::size_t& i)
    {
        discs_[i] = std::move(discs_.back());
        discs_.pop_back();
        --i;
    };

    for (std::size_t i = 0; i < discs_.size(); ++i)
    {
        auto& disc = discs_[i];
        if (disc.isMarkedDestroyed())
        {
            // This function also removes destroyed discs and thus needs to be called first
            swapAndPop(i);
            continue;
        }

        disc.move(disc.getVelocity() * dt);
        const auto discRadius = simulationContext_.discTypeRegistry.getByID(disc.getTypeID()).getRadius();

        // TODO re-use the collisions in the handling instead of finding them again there
        if (parent_)
        {
            const auto permeability = simulationContext_.membraneTypeRegistry.getByID(membrane_.getTypeID())
                                          .getPermeabilityFor(disc.getTypeID());
            if (permeability == MembraneType::Permeability::Bidirectional ||
                permeability == MembraneType::Permeability::Outward)
            {
                if (!mathutils::circlesOverlap(disc.getPosition(), discRadius, M, R))
                {
                    parent_->addDisc(disc);
                    swapAndPop(i);
                    continue;
                }
                else
                    parent_->addIntrudingDisc(disc);
            }
        }

        const auto leftX = disc.getPosition().x - discRadius;

        bool discWasDestroyed = false;
        for (const auto& compartmentEntry : compartmentEntries_)
        {
            if (leftX > compartmentEntry.rightX)
                break;

            const auto permeability = compartmentEntry.membraneType->getPermeabilityFor(disc.getTypeID());
            if (permeability == MembraneType::Permeability::None || permeability == MembraneType::Permeability::Outward)
                continue;

            if (mathutils::circleIsFullyContainedByCircle(disc.getPosition(), discRadius,
                                                          compartmentEntry.membrane->getPosition(),
                                                          compartmentEntry.membraneType->getRadius()))
            {
                compartmentEntry.compartment->addDisc(disc);
                swapAndPop(i);
                discWasDestroyed = true;
            }
            else
                compartmentEntry.compartment->addIntrudingDisc(disc);

            break;
        }

        if (discWasDestroyed)
            continue;

        // A -> B returns nothing, A -> B + C returns 1 new disc
        if (auto newDisc = simulationContext_.reactionEngine.applyUnimolecularReactions(disc, dt))
            newDiscs.push_back(std::move(*newDisc));
    }

    if (!newDiscs.empty())
        discs_.insert(discs_.end(), newDiscs.begin(), newDiscs.end());
}

void Compartment::updateChildCompartments(double dt)
{
    for (auto& compartment : compartments_)
        compartment->update(dt);
}

} // namespace cell