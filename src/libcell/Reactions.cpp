#include "Reactions.hpp"
#include "Disc.hpp"
#include "GlobalSettings.hpp"
#include "MathUtils.hpp"

#include <cmath>
#include <type_traits>

namespace cell
{

namespace
{
const auto& transformationReactionTable =
    GlobalSettings::getSettings().reactionTable_.getTransformationReactionLookupMap();

const auto& decompositionReactionTable =
    GlobalSettings::getSettings().reactionTable_.getDecompositionReactionLookupMap();

const auto& combinationReactionTable = GlobalSettings::getSettings().reactionTable_.getCombinationReactionLookupMap();

const auto& exchangeReactionTable = GlobalSettings::getSettings().reactionTable_.getExchangeReactionLookupMap();

const auto& simulationTimeStep = GlobalSettings::getSettings().simulationTimeStep_;

/**
 * @brief Helper function to search the lookup maps for reactions
 * @param map A reaction lookup map as returned by ReactionTable
 * @param key Either a single disc type (decomposition, transformation) or a pair of disc types (combination, exchange)
 */
template <typename MapType, typename KeyType> const Reaction* selectReaction(const MapType& map, const KeyType& key)
{
    const auto& iter = map.find(key);
    if (iter == map.end())
        return nullptr;

    const auto& possibleReactions = iter->second;
    double randomNumber = mathutils::getRandomFloat();

    // combination/exchange reactions always act on pairs of discs and don't have time based probabilities
    if constexpr (std::is_same_v<KeyType, std::pair<DiscType, DiscType>>)
    {
        for (const auto& reaction : possibleReactions)
        {
            if (randomNumber > reaction.getProbability())
                continue;

            return &reaction;
        }
    }
    else
    {
        const auto dt = simulationTimeStep.asSeconds();
        for (const auto& reaction : possibleReactions)
        {
            if (randomNumber > 1 - std::pow(1 - reaction.getProbability(), dt))
                continue;

            return &reaction;
        }
    }

    return nullptr;
}

} // namespace

bool transformationReaction(Disc* disc)
{
    const Reaction* reaction = selectReaction(transformationReactionTable, disc->getType());
    if (!reaction)
        return false;

    disc->setType(reaction->getProduct1());

    return true;
}

bool decompositionReaction(Disc* d1, std::vector<Disc>& newDiscs)
{
    const Reaction* reaction = selectReaction(decompositionReactionTable, d1->getType());
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

bool combinationReaction(Disc* d1, Disc* d2)
{
    const Reaction* reaction = selectReaction(combinationReactionTable, std::make_pair(d1->getType(), d2->getType()));
    if (!reaction)
        return false;

    const auto& resultType = reaction->getProduct1();

    // For reactions of type A + B -> C, we keep the one closer in size to C and destroy the other
    if (std::abs(resultType->getRadius() - d1->getType()->getRadius()) >
        std::abs(resultType->getRadius() - d2->getType()->getRadius()))
        std::swap(d1, d2);

    d1->setVelocity((d1->getType()->getMass() * d1->getVelocity() + d2->getType()->getMass() * d2->getVelocity()) /
                    resultType->getMass());
    d1->setType(resultType);

    d2->markDestroyed();

    return true;
}

bool exchangeReaction(Disc* d1, Disc* d2)
{
    const Reaction* reaction = selectReaction(exchangeReactionTable, std::make_pair(d1->getType(), d2->getType()));
    if (!reaction)
        return false;

    d1->scaleVelocity(std::sqrt(d1->getType()->getMass() / reaction->getProduct1()->getMass()));
    d1->setType(reaction->getProduct1());

    d2->scaleVelocity(std::sqrt(d2->getType()->getMass() / reaction->getProduct2()->getMass()));
    d2->setType(reaction->getProduct2());

    return true;
}

std::vector<Disc> unimolecularReactions(std::vector<Disc>& discs)
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