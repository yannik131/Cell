#ifndef REACTIONENGINE_HPP
#define REACTIONENGINE_HPP

#include "AbstractReactionTable.hpp"
#include "CollisionDetector.hpp"
#include "MathUtils.hpp"

#include <functional>
#include <optional>
#include <unordered_set>

namespace cell
{

class DiscType;
class Disc;

using SingleLookupMap = DiscTypeMap<std::vector<Reaction>>;
using PairLookupMap = DiscTypePairMap<std::vector<Reaction>>;

class ReactionEngine
{
public:
    ReactionEngine(const DiscTypeRegistry& discTypeRegistry, const AbstractReactionTable& reactionTable);

    /**
     * @brief Transformation reaction A -> B. Changes the type of the disc to a new one if a reaction occurs.
     * @param d1 The disc to transform
     */
    bool transformationReaction(Disc* d1, double dt) const;

    /**
     * @brief Decomposition reaction A -> B + C.
     * @param d1 The disc to decompose
     */
    std::optional<Disc> decompositionReaction(Disc* d1, double dt) const;

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
    std::optional<Disc> applyUnimolecularReactions(Disc& disc, double dt) const;

    void applyBimolecularReactions(const std::vector<CollisionDetector::Collision>& collisions) const;

private:
    template <typename MapType, typename KeyType, typename Condition>
    const Reaction* selectReaction(const MapType& map, const KeyType& key, const Condition& condition) const;

    const Reaction* selectUnimolecularReaction(const SingleLookupMap& map, const DiscTypeID& key, double dt) const;
    const Reaction* selectBimolecularReaction(const PairLookupMap& map,
                                              const std::pair<DiscTypeID, DiscTypeID>& key) const;

private:
    const DiscTypeRegistry& discTypeRegistry_;
    const SingleLookupMap* transformations_;
    const SingleLookupMap* decompositions_;
    const PairLookupMap* combinations_;
    const PairLookupMap* exchanges_;
};

template <typename MapType, typename KeyType, typename Condition>
inline const Reaction* ReactionEngine::selectReaction(const MapType& map, const KeyType& key,
                                                      const Condition& condition) const
{
    auto iter = map.find(key);
    if (iter == map.end())
        return nullptr;

    const auto& reactions = iter->second;
    auto start = mathutils::getRandomNumber<std::size_t>(0, reactions.size() - 1);

    for (std::size_t i = 0; i < reactions.size(); ++i)
    {
        const auto& reaction = reactions[(start + i) % reactions.size()];
        if (condition(reaction))
            return &reaction;
    }

    return nullptr;
}

} // namespace cell

#endif /* REACTIONENGINE_HPP */
