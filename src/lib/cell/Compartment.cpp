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

void Compartment::addIntrudingDisc(Disc& disc, const Compartment* source)
{
    intrudingDiscs_.push_back(&disc);

    const auto discRadius = simulationContext_.discTypeRegistry.getByID(disc.getTypeID()).getRadius();
    for (auto& compartment : compartments_)
    {
        if (compartment.get() == source)
            continue;

        // TODO In case of many compartments, make search more efficient
        const auto* membraneType =
            &simulationContext_.membraneTypeRegistry.getByID(compartment->getMembrane().getTypeID());
        if (mathutils::circlesOverlap(disc.getPosition(), discRadius, compartment->getMembrane().getPosition(),
                                      membraneType->getRadius()))
            compartment->addIntrudingDisc(disc, source);
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

        if (collision.type == CollisionDetector::CollisionType::DiscContainingMembrane)
            parent_->addIntrudingDisc(*collision.disc, this);
        else
            collision.membrane->getCompartment()->addIntrudingDisc(*collision.disc, nullptr);
    }
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
    registerIntruders(discMembraneCollisions);

    for (auto& compartment : compartments_)
        compartment->bimolecularUpdate();

    auto discDiscCollisions = detectDiscDiscCollisions();
    simulationContext_.reactionEngine.applyBimolecularReactions(discDiscCollisions);
    simulationContext_.collisionHandler.resolveCollisions(discMembraneCollisions);
    simulationContext_.collisionHandler.resolveCollisions(discDiscCollisions);

    captureIntruders();
}

void Compartment::unimolecularUpdate(double dt)
{
    moveDiscsAndCleanUp(dt);

    for (auto& compartment : compartments_)
        compartment->unimolecularUpdate(dt);
}

void Compartment::captureIntruders()
{
    const auto R = simulationContext_.membraneTypeRegistry.getByID(membrane_.getTypeID()).getRadius();
    const auto& M = membrane_.getPosition();

    for (auto& intruder : intrudingDiscs_)
    {
        if (intruder->isMarkedDestroyed())
            continue;

        const auto discRadius = simulationContext_.discTypeRegistry.getByID(intruder->getTypeID()).getRadius();

        if (mathutils::circleIsFullyContainedByCircle(intruder->getPosition(), discRadius, M, R))
        {
            discs_.push_back(*intruder);
            intruder->markDestroyed();
        }
    }

    intrudingDiscs_.clear();
}

void Compartment::updateChildCompartments(double dt)
{
    for (auto& compartment : compartments_)
        compartment->update(dt);
}

} // namespace cell