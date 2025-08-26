#include "ReactionEngine.hpp"
#include "Disc.hpp"
#include "MathUtils.hpp"
#include "Reaction.hpp"

namespace cell
{

ReactionEngine::ReactionEngine(DiscTypeResolver discTypeResolver, SimulationTimeStepProvider simulationTimeStepProvider,
                               const SingleLookupMap& decompositions, const SingleLookupMap& transformations,
                               const PairLookupMap& combinations, const PairLookupMap& exchanges)
    : discTypeResolver_(std::move(discTypeResolver))
    , simulationTimeStepProvider_(std::move(simulationTimeStepProvider))
    , decompositions_(&decompositions)
    , transformations_(&transformations)
    , combinations_(&combinations)
    , exchanges_(&exchanges)
{
}

bool ReactionEngine::transformationReaction(Disc* disc) const
{
    const Reaction* reaction = selectReaction(*transformations_, disc->getDiscTypeID());
    if (!reaction)
        return false;

    disc->setType(reaction->getProduct1());

    return true;
}

bool ReactionEngine::decompositionReaction(Disc* d1, std::vector<Disc>& newDiscs) const
{
    const Reaction* reaction = selectReaction(*decompositions_, d1->getDiscTypeID());
    if (!reaction)
        return false;

    const auto& vVec = d1->getVelocity();
    const double v = mathutils::abs(vVec);
    const sf::Vector2d n = vVec / v;

    // We will let the collision handling in the next time step take care of separation
    // But we can't have identical positions, so this ASSUMES that discs will be moved BEFORE the next collision
    // handling
    Disc product1(reaction->getProduct1());
    product1.setPosition(d1->getPosition());
    product1.setVelocity(v * sf::Vector2d{-n.y, n.x});

    Disc product2(reaction->getProduct2());
    product2.setPosition(d1->getPosition());
    product2.setVelocity(v * sf::Vector2d{n.y, -n.x});

    newDiscs.push_back(std::move(product1));
    newDiscs.push_back(std::move(product2));
    d1->markDestroyed();

    return true;
}

bool ReactionEngine::combinationReaction(Disc* d1, Disc* d2) const
{
    const Reaction* reaction = selectReaction(*combinations_, std::make_pair(d1->getDiscTypeID(), d2->getDiscTypeID()));
    if (!reaction)
        return false;

    const auto& resultType = discTypeResolver_(reaction->getProduct1());
    const auto& d1Type = discTypeResolver_(d1->getDiscTypeID());
    const auto& d2Type = discTypeResolver_(d2->getDiscTypeID());

    // For reactions of type A + B -> C, we keep the one closer in size to C and destroy the other
    if (std::abs(resultType.getRadius() - d1Type.getRadius()) > std::abs(resultType.getRadius() - d2Type.getRadius()))
        std::swap(d1, d2);

    d1->setVelocity((d1Type.getMass() * d1->getVelocity() + d2Type.getMass() * d2->getVelocity()) /
                    resultType.getMass());
    d1->setType(reaction->getProduct1());

    d2->markDestroyed();

    return true;
}

bool ReactionEngine::exchangeReaction(Disc* d1, Disc* d2) const
{
    const Reaction* reaction = selectReaction(*exchanges_, std::make_pair(d1->getDiscTypeID(), d2->getDiscTypeID()));
    if (!reaction)
        return false;

    const auto& d1Type = discTypeResolver_(d1->getDiscTypeID());
    const auto& d2Type = discTypeResolver_(d2->getDiscTypeID());
    const auto& product1Type = discTypeResolver_(reaction->getProduct1());
    const auto& product2Type = discTypeResolver_(reaction->getProduct2());

    d1->scaleVelocity(std::sqrt(d1Type.getMass() / product1Type.getMass()));
    d1->setType(reaction->getProduct1());

    d2->scaleVelocity(std::sqrt(d2Type.getMass() / product2Type.getMass()));
    d2->setType(reaction->getProduct2());

    return true;
}

std::vector<Disc> ReactionEngine::applyUnimolecularReactions(std::vector<Disc>& discs) const
{
    std::vector<Disc> newDiscs;

    for (auto& disc : discs)
    {
        if (!decompositionReaction(&disc, newDiscs))
            transformationReaction(&disc);
    }

    return newDiscs;
}

} // namespace cell