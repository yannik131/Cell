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
    simulationContext_.collisionDetector.setParams(CollisionDetector::Params{.discs = &discs_,
                                                                             .membranes = &membranes_,
                                                                             .intrudingDiscs = &intrudingDiscs_,
                                                                             .containingMembrane = &membrane_});
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

void Compartment::addIntrudingDisc(Disc& disc, SearchChildren searchChildren)
{
    intrudingDiscs_.push_back(&disc);

    if (!searchChildren.value)
        return;

    const auto discRadius = simulationContext_.discTypeRegistry.getByID(disc.getTypeID()).getRadius();
    const auto leftX = disc.getPosition().x - discRadius;

    for (const auto& compartmentEntry : compartmentEntries_)
    {
        if (leftX > compartmentEntry.rightX)
            break;

        const auto* membraneType =
            &simulationContext_.membraneTypeRegistry.getByID(compartmentEntry.membrane->getTypeID());
        if (mathutils::circlesOverlap(disc.getPosition(), discRadius, compartmentEntry.membrane->getPosition(),
                                      membraneType->getRadius()))
            compartmentEntry.compartment->addIntrudingDisc(disc, searchChildren);
    }
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
    auto collisions = simulationContext_.collisionDetector.detectCollisions();
    intrudingDiscs_.clear();
    return collisions;
}

void Compartment::update(double dt)
{
    /**
     * 1st iteration:
     *  - Find disc-membrane collisions
     * END
     * - Add intruders to parent/child (for child: set flag for recursive overlap search with deeper children) or move
     * to parent/child
     * - Recurse into child compartments
     * 2nd iteration:
     * - Find disc-disc collisions
     * END
     * - Clear intruding discs
     * - Apply reactions
     * - Resolve collisions
     * 3rd iteration:
     * - Remove destroyed discs
     * - Move discs
     * END
     */
    auto discMembraneCollisions = detectDiscMembraneCollisions();
    moveDiscsBetweenCompartments(discMembraneCollisions);
    updateChildCompartments(dt);

    auto discDiscCollisions = detectDiscDiscCollisions();
    simulationContext_.reactionEngine.applyBimolecularReactions(discDiscCollisions);
    simulationContext_.collisionHandler.resolveCollisions(discMembraneCollisions, discDiscCollisions);

    moveDiscsAndCleanUp(dt);
}

Compartment* Compartment::createSubCompartment(Membrane membrane)
{
    auto compartment = std::make_unique<Compartment>(this, std::move(membrane), simulationContext_);
    membranes_.push_back(compartment->getMembrane());
    compartments_.push_back(std::move(compartment));

    simulationContext_.collisionDetector.buildMembraneIndex(membranes_);

    return compartments_.back().get();
}

void Compartment::moveDiscsAndApplyUnimolecularReactions(double dt)
{
    std::vector<Disc> newDiscs;

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
            swapAndPop(i);
            continue;
        }

        disc.move(disc.getVelocity() * dt);

        if (moveDiscToParentCompartment(disc) || moveDiscToChildCompartment(disc))
        {
            swapAndPop(i);
            continue;
        }

        // A -> B returns nothing, A -> B + C returns 1 new disc
        if (auto newDisc = simulationContext_.reactionEngine.applyUnimolecularReactions(disc, dt))
            newDiscs.push_back(std::move(*newDisc));
    }

    if (!newDiscs.empty())
        discs_.insert(discs_.end(), newDiscs.begin(), newDiscs.end());
}

auto Compartment::detectDiscMembraneCollisions()
{
    simulationContext_.collisionDetector.buildDiscIndex();
    return simulationContext_.collisionDetector.detectDiscMembraneCollisions();
}

auto Compartment::detectDiscDiscCollisions()
{
    simulationContext_.collisionDetector.addIntrudingDiscsToIndex();
    return simulationContext_.collisionDetector.detectDiscDiscCollisions();
}

void Compartment::updateChildCompartments(double dt)
{
    for (auto& compartment : compartments_)
        compartment->update(dt);
}

bool Compartment::moveDiscToParentCompartment(Disc& disc)
{
    if (!parent_)
        return false;

    const auto permeability =
        simulationContext_.membraneTypeRegistry.getByID(membrane_.getTypeID()).getPermeabilityFor(disc.getTypeID());
    if (permeability == MembraneType::Permeability::Bidirectional ||
        permeability == MembraneType::Permeability::Outward)
    {
        const auto& membraneType = simulationContext_.membraneTypeRegistry.getByID(membrane_.getTypeID());
        const auto discRadius = simulationContext_.discTypeRegistry.getByID(disc.getTypeID()).getRadius();

        if (!mathutils::circlesOverlap(disc.getPosition(), discRadius, membrane_.getPosition(),
                                       membraneType.getRadius()))
        {
            parent_->addDisc(disc);
            return true;
        }
        else
            parent_->addIntrudingDisc(disc, SearchChildren{false});
    }

    return false;
}

bool Compartment::moveDiscToChildCompartment(Disc& disc)
{
    const auto discRadius = simulationContext_.discTypeRegistry.getByID(disc.getTypeID()).getRadius();
    const auto leftX = disc.getPosition().x - discRadius;

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
            return true;
        }
        else
            compartmentEntry.compartment->addIntrudingDisc(disc, SearchChildren{true});

        break;
    }

    return false;
}

} // namespace cell