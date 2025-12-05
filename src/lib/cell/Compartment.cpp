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
    , collisionDetector_(simulationContext.discTypeRegistry, simulationContext.membraneTypeRegistry)
{
    membrane_.setCompartment(this);
    collisionDetector_.setParams(CollisionDetector::Params{.discs = &discs_,
                                                           .membranes = &membranes_,
                                                           .intrudingDiscs = &intrudingDiscs_,
                                                           .containingMembrane = &membrane_});
}

Compartment::~Compartment() = default;

const Membrane& Compartment::getMembrane() const
{
    return membrane_;
}

void Compartment::setDiscs(std::deque<Disc>&& discs)
{
    discs_ = std::move(discs);
}

void Compartment::addDisc(Disc disc)
{
    discs_.push_back(std::move(disc));
}

const std::deque<Disc>& Compartment::getDiscs() const
{
    return discs_;
}

void Compartment::addIntrudingDisc(Disc& disc, SearchChildren searchChildren)
{
    intrudingDiscs_.push_back(&disc);

    if (!searchChildren.value)
        return;

    const auto discRadius = simulationContext_.discTypeRegistry.getByID(disc.getTypeID()).getRadius();
    for (auto& compartment : compartments_)
    {
        // TODO In case of many compartments, make search more efficient
        const auto* membraneType =
            &simulationContext_.membraneTypeRegistry.getByID(compartment->getMembrane().getTypeID());
        if (mathutils::circlesOverlap(disc.getPosition(), discRadius, compartment->getMembrane().getPosition(),
                                      membraneType->getRadius()))
            compartment->addIntrudingDisc(disc, SearchChildren{true});
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

auto Compartment::detectDiscMembraneCollisions()
{
    collisionDetector_.buildDiscIndex(); // TODO Already
    return collisionDetector_.detectDiscMembraneCollisions();
}

auto Compartment::detectDiscDiscCollisions()
{
    collisionDetector_.addIntrudingDiscsToIndex();
    auto collisions = collisionDetector_.detectDiscDiscCollisions();

    return collisions;
}

void Compartment::update(double dt)
{
    // TODO Remove recursing twice and just accept destroyed discs at the end of update?
    bimolecularUpdate();
    unimolecularUpdate(dt);
}

Compartment* Compartment::createSubCompartment(Membrane membrane)
{
    auto compartment = std::make_unique<Compartment>(this, std::move(membrane), simulationContext_);
    membranes_.push_back(compartment->getMembrane());
    compartments_.push_back(std::move(compartment));
    collisionDetector_.buildMembraneIndex();

    return compartments_.back().get();
}

void Compartment::moveDiscsAndCleanUp(double dt)
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

        // A -> B returns nothing, A -> B + C returns 1 new disc
        if (auto newDisc = simulationContext_.reactionEngine.applyUnimolecularReactions(disc, dt))
            newDiscs.push_back(std::move(*newDisc));

        disc.move(disc.getVelocity() * dt);
    }

    if (!newDiscs.empty())
        discs_.insert(discs_.end(), newDiscs.begin(), newDiscs.end());
}

void Compartment::bimolecularUpdate()
{
    auto discMembraneCollisions = detectDiscMembraneCollisions();
    moveDiscsBetweenCompartments(discMembraneCollisions);

    for (auto& compartment : compartments_)
        compartment->bimolecularUpdate();

    auto discDiscCollisions = detectDiscDiscCollisions();
    simulationContext_.reactionEngine.applyBimolecularReactions(discDiscCollisions);
    simulationContext_.collisionHandler.resolveCollisions(discMembraneCollisions);
    simulationContext_.collisionHandler.resolveCollisions(discDiscCollisions);

    intrudingDiscs_.clear();
}

void Compartment::unimolecularUpdate(double dt)
{
    moveDiscsAndCleanUp(dt);

    for (auto& compartment : compartments_)
        compartment->unimolecularUpdate(dt);
}

void Compartment::moveDiscsBetweenCompartments(const std::vector<CollisionDetector::Collision>& discMembraneCollisions)
{
    const MembraneType* parentMembraneType = nullptr;
    if (parent_)
        parentMembraneType = &simulationContext_.membraneTypeRegistry.getByID(parent_->getMembrane().getTypeID());

    for (const auto& collision : discMembraneCollisions)
    {
        if (collision.invalidatedByDestroyedDisc() || !collision.allowedToPass)
            continue;

        const auto discRadius = simulationContext_.discTypeRegistry.getByID(collision.disc->getTypeID()).getRadius();
        const auto membraneRadius =
            simulationContext_.membraneTypeRegistry.getByID(collision.membrane->getTypeID()).getRadius();

        switch (collision.type)
        {
        case CollisionDetector::CollisionType::DiscContainingMembrane:
            if (!parent_)
                continue;

            if (!mathutils::circleIsFullyContainedByCircle(collision.disc->getPosition(), discRadius,
                                                           collision.membrane->getPosition(), membraneRadius))
            {
                parent_->addDisc(*collision.disc);
                collision.disc->markDestroyed();
            }
            else
                parent_->addIntrudingDisc(*collision.disc,
                                          SearchChildren{false}); // TODO Enable search without bad recursion
            break;
        case CollisionDetector::CollisionType::DiscChildMembrane:
            if (mathutils::circleIsFullyContainedByCircle(collision.disc->getPosition(), discRadius,
                                                          collision.membrane->getPosition(), membraneRadius))
            {
                collision.membrane->getCompartment()->addDisc(*collision.disc);
                collision.disc->markDestroyed();
            }
            else
                collision.membrane->getCompartment()->addIntrudingDisc(*collision.disc, SearchChildren{true});
            break;
        default: return; // Silent fail
        }
    }
}

void Compartment::updateChildCompartments(double dt)
{
    for (auto& compartment : compartments_)
        compartment->update(dt);
}

} // namespace cell