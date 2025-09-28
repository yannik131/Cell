#include "Compartment.hpp"
#include "CollisionDetector.hpp"
#include "CollisionHandler.hpp"
#include "Disc.hpp"
#include "MathUtils.hpp"
#include "ReactionEngine.hpp"

namespace cell
{

Compartment::Compartment(Compartment* parent, Membrane&& membrane, std::vector<Membrane>& membranes,
                         SimulationContext simulationContext)
    : parent_(parent)
    , membrane_(std::move(membrane))
    , simulationContext_(std::move(simulationContext))
{
    auto getMembraneType = [&](const Membrane& membrane) -> const MembraneType&
    { return simulationContext_.membraneTypeRegistry.getByID(membrane.getMembraneTypeID()); };

    const auto M = membrane_.getPosition();
    const auto R = getMembraneType(membrane_).getRadius();

    std::vector<Membrane> containedMembranes;
    for (auto iter = membranes.begin(); iter != membranes.end();)
    {
        const auto Mo = iter->getPosition();
        const auto Ro = getMembraneType(*iter).getRadius();

        if (mathutils::circleIsFullyContainedByCircle(Mo, Ro, M, R))
        {
            containedMembranes.push_back(std::move(*iter));
            iter = membranes.erase(iter);
            continue;
        }
        ++iter;
    }

    while (!containedMembranes.empty())
    {
        auto containedMembrane = std::move(containedMembranes.back());
        containedMembranes.pop_back();
        compartments_.emplace_back(this, std::move(containedMembrane), containedMembranes, simulationContext_);
    }
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

const std::vector<Compartment>& Compartment::getCompartments() const
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
        compartment.update(dt);
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