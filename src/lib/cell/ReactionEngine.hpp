#ifndef REACTIONENGINE_HPP
#define REACTIONENGINE_HPP

#include "CollisionDetector.hpp"
#include "MathUtils.hpp"

#include <functional>
#include <optional>
#include <unordered_set>

namespace cell
{

class DiscType;
class Disc;
class ReactionTable;

using SingleLookupMap = DiscTypeMap<std::vector<Reaction>>;
using PairLookupMap = DiscTypePairMap<std::vector<Reaction>>;

class ReactionEngine
{
public:
    ReactionEngine(const DiscTypeRegistry& discTypeRegistry, const ReactionTable& reactionTable);

    /**
     * @brief Transformation reaction A -> B. Changes the type of the disc to a new one if a reaction occurs.
     */
    Disc transformationReaction(Disc* educt, DiscTypeID productID) const;

    /**
     * @brief Decomposition reaction A -> B + C.
     */
    std::pair<Disc, Disc> decompositionReaction(Disc* educt, DiscTypeID product1ID, DiscTypeID product2ID) const;

    /**
     * @brief Combination reaction A + B -> C. Destroys one of the 2 educt discs and changes the other if a reaction
     * occurs.
     */
    Disc combinationReaction(Disc* educt1, Disc* educt2, DiscTypeID productID) const;

    /**
     * @brief Exchange reaction A + B -> C + D. Just changes the disc types of the reacting discs.
     */
    std::pair<Disc, Disc> exchangeReaction(Disc* educt1, Disc* educt2, DiscTypeID product1ID,
                                           DiscTypeID product2ID) const;

    void applyUnimolecularReactions(Disc& disc, double dt, std::vector<Disc>& newDiscs) const;

    void applyBimolecularReactions(const std::vector<CollisionDetector::Collision>& collisions,
                                   std::vector<Disc>& newDiscs) const;

private:
    template <typename MapType, typename KeyType, typename Condition>
    const Reaction* selectReaction(const MapType& map, const KeyType& key, const Condition& condition) const;

    const Reaction* selectUnimolecularReaction(const DiscTypeID& key, double dt) const;
    const Reaction* selectBimolecularReaction(const std::pair<DiscTypeID, DiscTypeID>& key) const;
    void combineReactionsIntoSingleMaps(const ReactionTable& reactionTable);

private:
    const DiscTypeRegistry& discTypeRegistry_;
    SingleLookupMap unimolecularReactions_;
    PairLookupMap bimolecularReactions_;
};

template <typename MapType, typename KeyType, typename Condition>
inline const Reaction* ReactionEngine::selectReaction(const MapType& map, const KeyType& key,
                                                      const Condition& condition) const
{
    auto iter = map.find(key);
    if (iter == map.end())
        return nullptr;

    const auto& reactions = iter->second;

    // ReactionTable never constructs empty vectors, so reaction.size() is always > 0
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
