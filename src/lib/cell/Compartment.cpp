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
    , collisionDetector_(simulationContext_.discTypeRegistry, simulationContext_.membraneTypeRegistry)
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

void Compartment::addIntrudingDisc(Disc* disc, const Compartment* source, bool shouldBeCaptured)
{
    intrudingDiscs_.push_back(disc);
    intruderCaptureStatus_.push_back(static_cast<char>(shouldBeCaptured));

    if (compartments_.size() <= 1)
        return;

    const auto discRadius = simulationContext_.discTypeRegistry.getByID(disc->getTypeID()).getRadius();
    for (auto& compartment : compartments_)
    {
        if (compartment.get() == source)
            continue;

        // TODO In case of many compartments, make search more efficient
        const auto* membraneType =
            &simulationContext_.membraneTypeRegistry.getByID(compartment->getMembrane().getTypeID());
        if (mathutils::circlesOverlap(disc->getPosition(), discRadius, compartment->getMembrane().getPosition(),
                                      membraneType->getRadius()))
            compartment->addIntrudingDisc(disc, source, shouldBeCaptured);
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

void Compartment::update(double dt)
{
    // TODO Remove recursing twice and just accept destroyed discs at the end of update?
    bimolecularUpdate();
    unimolecularUpdate(dt);
}

void Compartment::bimolecularUpdate()
{
    allocateMemoryForIntruders();
    auto discMembraneCollisions = detectDiscMembraneCollisions();
    registerIntruders(discMembraneCollisions);

    for (auto& compartment : compartments_)
        compartment->bimolecularUpdate();

    auto discDiscCollisions = detectDiscDiscCollisions();
    simulationContext_.collisionHandler.resolveCollisions(discMembraneCollisions);
    simulationContext_.collisionHandler.resolveCollisions(discDiscCollisions);
    simulationContext_.reactionEngine.applyBimolecularReactions(discDiscCollisions, newDiscs_);

    captureIntruders();
}

void Compartment::unimolecularUpdate(double dt)
{
    for (auto& compartment : compartments_)
        compartment->unimolecularUpdate(dt);

    moveDiscsAndCleanUp(dt);
}

void Compartment::allocateMemoryForIntruders()
{
    if (!needMoreMemoryForIntruders_)
        return;

    discs_.reserve(std::max(discs_.size() * 2, static_cast<std::size_t>(2))); // Let's be generous here
    needMoreMemoryForIntruders_ = false;
}

Compartment* Compartment::createSubCompartment(Membrane membrane)
{
    auto compartment = std::make_unique<Compartment>(this, std::move(membrane), simulationContext_);
    membranes_.push_back(compartment->getMembrane());
    compartments_.push_back(std::move(compartment));
    collisionDetector_.buildMembraneIndex();

    return compartments_.back().get();
}

std::vector<cell::CollisionDetector::Collision> Compartment::detectDiscMembraneCollisions()
{
    collisionDetector_.buildDiscIndex();
    return collisionDetector_.detectDiscMembraneCollisions();
}

std::vector<cell::CollisionDetector::Collision> Compartment::detectDiscDiscCollisions()
{
    collisionDetector_.addIntrudingDiscsToIndex();
    auto collisions = collisionDetector_.detectDiscDiscCollisions();

    return collisions;
}

void Compartment::registerIntruders(const std::vector<CollisionDetector::Collision>& discMembraneCollisions)
{
    // Assumptions:
    // - Called directly after collision detection, so no destroyed discs yet
    // - parent_ can't be nullptr because the outermost membrane shouldn't be permeable for anything so
    // collision.allowedToPass would always be false
    // - Also this function only expects DiscContainingMembrane and DiscChildMembrane collisions

    for (const auto& collision : discMembraneCollisions)
    {
        if (!collision.allowedToPass)
            continue;

        const auto discRadius = simulationContext_.discTypeRegistry.getByID(collision.disc->getTypeID()).getRadius();

        if (collision.type == CollisionDetector::CollisionType::DiscContainingMembrane)
        {
            const auto containingMembranePosition = parent_->getMembrane().getPosition();
            const auto containingMembraneRadius =
                simulationContext_.membraneTypeRegistry.getByID(parent_->getMembrane().getTypeID()).getRadius();
            const bool shouldBeCaptured = !mathutils::circlesOverlap(
                collision.disc->getPosition(), discRadius, containingMembranePosition, containingMembraneRadius);

            parent_->addIntrudingDisc(collision.disc, this, shouldBeCaptured);
        }
        else
        {
            const auto M = collision.membrane->getPosition();
            const auto membraneRadius =
                simulationContext_.membraneTypeRegistry.getByID(collision.membrane->getTypeID()).getRadius();
            const bool shouldBeCaptured =
                mathutils::circleIsFullyContainedByCircle(collision.disc->getPosition(), discRadius, M, membraneRadius);

            collision.membrane->getCompartment()->addIntrudingDisc(collision.disc, nullptr, shouldBeCaptured);
        }
    }
}

void Compartment::captureIntruders()
{
    // Intruders can safely be added without reallocation (which would invalidate any pointers/intruders in other
    // compartments)
    if (discs_.capacity() >= discs_.size() + intrudingDiscs_.size())
    {
        for (std::size_t i = 0; i < intrudingDiscs_.size(); ++i)
        {
            auto& intruder = intrudingDiscs_[i];
            if (intruder->isMarkedDestroyed())
                continue;

            if (intruderCaptureStatus_[i])
            {
                discs_.push_back(*intruder);
                intruder->markDestroyed();
            }
        }
    }
    else
        needMoreMemoryForIntruders_ = true; // Better luck next time

    intrudingDiscs_.clear();
    intruderCaptureStatus_.clear();
}

void Compartment::moveDiscsAndCleanUp(double dt)
{
    for (std::size_t i = 0; i < discs_.size(); ++i)
    {
        auto& disc = discs_[i];
        simulationContext_.reactionEngine.applyUnimolecularReactions(disc, dt, newDiscs_);

        if (disc.isMarkedDestroyed())
        {
            discs_[i] = std::move(discs_.back());
            discs_.pop_back();
            --i;
            continue;
        }

        disc.move(disc.getVelocity() * dt);
    }

    for (auto& disc : newDiscs_)
        disc.move(disc.getVelocity() * dt);

    if (!newDiscs_.empty())
    {
        discs_.insert(discs_.end(), newDiscs_.begin(), newDiscs_.end());
        newDiscs_.clear();
    }
}

} // namespace cell