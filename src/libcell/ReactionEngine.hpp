#ifndef REACTIONENGINE_HPP
#define REACTIONENGINE_HPP

#include "AbstractReactionTable.hpp"
#include "DiscTypeRegistry.hpp"
#include "MathUtils.hpp"

#include <functional>
#include <optional>
#include <set>

namespace cell
{

class DiscType;
class Disc;

class ReactionEngine
{
public:
    using SingleLookupMap = DiscTypeMap<std::vector<Reaction>>;
    using PairLookupMap = DiscTypePairMap<std::vector<Reaction>>;

public:
    ReactionEngine(DiscTypeResolver discTypeResolver, SimulationTimeStepProvider simulationTimeStepProvider,
                   const AbstractReactionTable& reactionTable);

    /**
     * @brief Transformation reaction A -> B. Changes the type of the disc to a new one if a reaction occurs.
     * @param d1 The disc to transform
     */
    bool transformationReaction(Disc* d1) const;

    /**
     * @brief Decomposition reaction A -> B + C.
     * @param d1 The disc to decompose
     */
    std::optional<Disc> decompositionReaction(Disc* d1) const;

    /**
     * @brief Combination reaction A + B -> C. Destroys one of the 2 educt discs and changes the other if a reaction
     * occurs.
     * @param d1 Colliding disc 1
     * @param d2 Colliding disc 2
     */
    bool combinationReaction(Disc* d1, Disc* d2) const;

    /**
     * @brief Exchange reaction A + B -> C + D. Just changes the disc types of the reacting discs.
     * @param d1 Colliding disc 1
     * @param d2 Colliding disc 2
     */
    bool exchangeReaction(Disc* d1, Disc* d2) const;

    /**
     * @brief Lets decomposition and transformation reactions occur for the passed discs. Only 1 of the 2 reaction types
     * will occur for each disc.
     * @param discs Discs to transform/decompose
     */
    std::optional<Disc> applyUnimolecularReactions(Disc& disc) const;

    void applyBimolecularReactions(std::set<std::pair<Disc*, Disc*>> collidingDiscs) const;

private:
    /**
     * @brief Helper function to search the lookup maps for reactions
     * @param map A reaction lookup map as returned by ReactionTable
     * @param key Either a single disc type (decomposition, transformation) or a pair of disc types (combination,
     * exchange)
     */
    template <typename MapType, typename KeyType>
    const Reaction* selectReaction(const MapType& map, const KeyType& key) const;

private:
    DiscTypeResolver discTypeResolver_;
    SimulationTimeStepProvider simulationTimeStepProvider_;
    const SingleLookupMap* transformations_;
    const SingleLookupMap* decompositions_;
    const PairLookupMap* combinations_;
    const PairLookupMap* exchanges_;
};

template <typename MapType, typename KeyType>
const Reaction* ReactionEngine::selectReaction(const MapType& map, const KeyType& key) const
{
    const auto& iter = map.find(key);
    if (iter == map.end())
        return nullptr;

    const auto& possibleReactions = iter->second;

    // combination/exchange reactions always act on pairs of discs and don't have time based probabilities
    if constexpr (std::is_same_v<KeyType, std::pair<DiscTypeID, DiscTypeID>>)
    {
        for (const auto& reaction : possibleReactions)
        {
            if (mathutils::getRandomFloat() > reaction.getProbability())
                continue;

            return &reaction;
        }
    }
    else
    {
        const auto dt = simulationTimeStepProvider_();
        for (const auto& reaction : possibleReactions)
        {
            if (mathutils::getRandomFloat() > 1 - std::pow(1 - reaction.getProbability(), dt))
                continue;

            return &reaction;
        }
    }

    return nullptr;
}

} // namespace cell

#endif /* REACTIONENGINE_HPP */
