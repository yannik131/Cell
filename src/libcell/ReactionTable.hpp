#ifndef REACTIONTABLE_HPP
#define REACTIONTABLE_HPP

#include "DiscType.hpp"
#include "DiscTypeRegistry.hpp"
#include "Reaction.hpp"
#include "SFMLJsonSerializers.hpp"

#include <vector>

namespace cell
{

/**
 * @brief Adds a reaction to vector, making sure that the reactions are sorted by cumulative probability. Let X(p) be a
 * reaction with probability p:
 *
 * - {} + A(0.1) -> {A(0.1)}
 *
 * - {A(0.1)} + B(0.2) -> {A(0.1), B(0.3)}
 *
 * - {A(0.1), B(0.3)} + C(0.05) -> {A(0.1), B(0.3), C(0.35)}
 *
 * Let's say 2 discs collide and a reaction might occur because there is a list of possible reactions for the colliding
 * disc types. We generate a random number f between 0 and 1 and iterate the vector in order, checking if f is <= p for
 * the given reaction. I. e. if f = 0.2, reaction B would occur. If f = 0.5, no reaction would occur.
 */
void addReactionToVector(std::vector<Reaction>& reactions, Reaction reaction, const DiscTypeResolver& discTypeResolver);

/**
 * @brief Maps a `vector<Reaction>` with increasing, cumulative probabilities to `DiscType` or pairs of such:
 *
 * - `DiscType` -> transformation/decomposition reactions
 *
 * - `pair<DiscType, DiscType>` -> combination/exchange reactions
 */
class ReactionTable
{
public:
    ReactionTable(DiscTypeResolver discTypeResolver);

    // getters for lookup up maps for reactions with increasing, cumulative probabilities

    const DiscTypeMap<std::vector<Reaction>>& getTransformationReactionLookupMap() const;
    const DiscTypeMap<std::vector<Reaction>>& getDecompositionReactionLookupMap() const;
    const DiscTypePairMap<std::vector<Reaction>>& getCombinationReactionLookupMap() const;
    const DiscTypePairMap<std::vector<Reaction>>& getExchangeReactionLookupMap() const;

    /**
     * @brief Adds a new reaction to the table and updates all lookup maps
     */
    void addReaction(const Reaction& reaction);

    /**
     * @brief Equivalent to calling `clear()` and `add(...)` for each reaction
     */
    void setReactions(const std::vector<Reaction>& reactions);

    /**
     * @brief Removes all reactions where the given disc types are part of the educts or products
     * @param discTypesToRemove A vector containing disc types that are to be removed. Their IDs must match with the
     * disc types already in the table.
     */
    void removeDiscType(DiscTypeID discTypeToRemove);

    /**
     * @brief Clears the reaction vector and all reaction lookup tables
     */
    void clear();

    /**
     * @returns a vector containing all reactions currently in the table
     */
    const std::vector<Reaction>& getReactions() const;

private:
    /**
     * @brief Clears the old lookup maps and creates new ones based on the current reaction vector
     */
    void createLookupMaps();

private:
    std::vector<Reaction> reactions_;

    DiscTypeMap<std::vector<Reaction>> transformationReactionLookupMap_;
    DiscTypeMap<std::vector<Reaction>> decompositionReactionLookupMap_;
    DiscTypePairMap<std::vector<Reaction>> combinationReactionLookupMap_;
    DiscTypePairMap<std::vector<Reaction>> exchangeReactionLookupMap_;

    DiscTypeResolver discTypeResolver_;

public:
    /*     NLOHMANN_DEFINE_TYPE_INTRUSIVE(ReactionTable, reactions_, transformationReactionLookupMap_,
                                       decompositionReactionLookupMap_, combinationReactionLookupMap_,
                                       exchangeReactionLookupMap_) */
};

} // namespace cell

#endif /* REACTIONTABLE_HPP */
