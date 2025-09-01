#include "ReactionEngine.hpp"
#include "Disc.hpp"
#include "MathUtils.hpp"
#include "Reaction.hpp"

namespace cell
{

ReactionEngine::ReactionEngine(DiscTypeResolver discTypeResolver, SimulationTimeStepProvider simulationTimeStepProvider,
                               const AbstractReactionTable& reactionTable)
    : discTypeResolver_(std::move(discTypeResolver))
    , simulationTimeStepProvider_(std::move(simulationTimeStepProvider))
    , decompositions_(&reactionTable.getDecompositions())
    , transformations_(&reactionTable.getTransformations())
    , combinations_(&reactionTable.getCombinations())
    , exchanges_(&reactionTable.getExchanges())
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

std::optional<Disc> ReactionEngine::decompositionReaction(Disc* d1) const
{
    const Reaction* reaction = selectReaction(*decompositions_, d1->getDiscTypeID());
    if (!reaction)
        return {};

    double v = mathutils::abs(d1->getVelocity());
    if (v == 0)
    {
        // If the disc is stationary and wants to split apart, we'll give it a random velocity to do so
        d1->setVelocity(sf::Vector2d{mathutils::getRandomFloat(), mathutils::getRandomFloat()} * 10.0);
        v = mathutils::abs(d1->getVelocity());
    }

    const sf::Vector2d n = d1->getVelocity() / v;

    // We will let the collision handling in the next time step take care of separation
    d1->setType(reaction->getProduct1());
    d1->setVelocity(v * sf::Vector2d{-n.y, n.x});

    Disc product2(reaction->getProduct2());
    product2.setPosition(d1->getPosition());
    product2.setVelocity(v * sf::Vector2d{n.y, -n.x});

    return product2;
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

std::optional<Disc> ReactionEngine::applyUnimolecularReactions(Disc& disc) const
{
    // TODO random shuffle all reactions
    if (transformationReaction(&disc))
        return {};

    return decompositionReaction(&disc);
}

void ReactionEngine::applyBimolecularReactions(const std::set<std::pair<Disc*, Disc*>>& collidingDiscs) const
{
    for (auto& [d1, d2] : collidingDiscs)
    {
        if (combinationReaction(d1, d2))
            continue;
        exchangeReaction(d1, d2);
    }
}

} // namespace cell