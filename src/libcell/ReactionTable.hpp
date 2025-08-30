#ifndef REACTIONTABLE_HPP
#define REACTIONTABLE_HPP

#include "AbstractReactionTable.hpp"
#include "DiscType.hpp"
#include "DiscTypeRegistry.hpp"
#include "Reaction.hpp"
#include "SFMLJsonSerializers.hpp"

#include <vector>

namespace cell
{

/**
 * @brief Maps a `vector<Reaction>` to `DiscType` or pairs of such:
 *
 * - `DiscType` -> transformation/decomposition reactions
 *
 * - `pair<DiscType, DiscType>` -> combination/exchange reactions
 */
class ReactionTable : public AbstractReactionTable
{
public:
    ReactionTable(DiscTypeResolver discTypeResolver);

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
