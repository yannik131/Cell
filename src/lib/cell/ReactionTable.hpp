#ifndef EBF12521_1715_4669_88B2_124B8D0C2AEA_HPP
#define EBF12521_1715_4669_88B2_124B8D0C2AEA_HPP

#include "DiscType.hpp"
#include "Reaction.hpp"

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
class ReactionTable
{
public:
    ReactionTable(const DiscTypeRegistry& discTypeRegistry);

    const DiscTypeMap<std::vector<Reaction>>& getTransformations() const;
    const DiscTypeMap<std::vector<Reaction>>& getDecompositions() const;
    const DiscTypePairMap<std::vector<Reaction>>& getCombinations() const;
    const DiscTypePairMap<std::vector<Reaction>>& getExchanges() const;

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

    void checkIfIsDuplicateReaction(const Reaction& reaction) const;

    bool isUnary(const Reaction& r) const;

    // We make these helpers static and let them have *this as a parameter to avoid const overloads

    template <typename Self> static auto& unaryMap(Self& self, const Reaction& r)
    {
        return (r.getType() == Reaction::Type::Transformation) ? self.transformations_ : self.decompositions_;
    }

    template <typename Self> static auto& binaryMap(Self& self, const Reaction& r)
    {
        return (r.getType() == Reaction::Type::Combination) ? self.combinations_ : self.exchanges_;
    }

private:
    std::vector<Reaction> reactions_;

    DiscTypeMap<std::vector<Reaction>> transformations_;
    DiscTypeMap<std::vector<Reaction>> decompositions_;
    DiscTypePairMap<std::vector<Reaction>> combinations_;
    DiscTypePairMap<std::vector<Reaction>> exchanges_;

    const DiscTypeRegistry& discTypeRegistry_;
};

} // namespace cell

#endif /* EBF12521_1715_4669_88B2_124B8D0C2AEA_HPP */
