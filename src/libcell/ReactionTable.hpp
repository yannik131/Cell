#ifndef REACTIONTABLE_HPP
#define REACTIONTABLE_HPP

#include "DiscType.hpp"

#include <vector>

class Reaction;

class ReactionTable
{
public:
    const DiscType::map<std::vector<Reaction>>& getDecompositionReactionLookupMap() const;
    const DiscType::map<std::vector<Reaction>>& getTransformationReactionLookupMap() const;
    const DiscType::pair_map<std::vector<Reaction>>& getCombinationReactionLookupMap() const;
    const DiscType::pair_map<std::vector<Reaction>>& getExchangeReactionLookupMap() const;

    void addReaction(const Reaction& reaction);
    void setReactions(const std::vector<Reaction>& reactions);
    void updateDiscTypes(const DiscType::map<DiscType>& updatedDiscTypes);
    void removeDiscTypes(const std::vector<DiscType>& discTypesToRemove);
    void clear();

private:
    void createLookupMaps();

private:
    std::vector<Reaction> reactions_;

    DiscType::map<std::vector<Reaction>> decompositionReactionLookupMap_;
    DiscType::map<std::vector<Reaction>> transformationReactionLookupMap_;
    DiscType::pair_map<std::vector<Reaction>> combinationReactionLookupMap_;
    DiscType::pair_map<std::vector<Reaction>> exchangeReactionLookupMap_;
};

#endif /* REACTIONTABLE_HPP */
