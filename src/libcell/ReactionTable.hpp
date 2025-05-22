#ifndef REACTIONTABLE_HPP
#define REACTIONTABLE_HPP

#include <unordered_map>
#include <vector>

class Reaction;
class DiscType;

class ReactionTable
{
public:
    const std::unordered_map<DiscType, std::vector<Reaction>>& getDecompositionReactionLookupMap() const;
    const std::unordered_map<DiscType, std::vector<Reaction>>& getTransformationReactionLookupMap() const;
    const std::unordered_map<std::pair<DiscType, DiscType>, std::vector<Reaction>>&
    getCombinationReactionLookupMap() const;
    const std::unordered_map<std::pair<DiscType, DiscType>, std::vector<Reaction>>&
    getExchangeReactionLookupMap() const;

    void addReaction(const Reaction& reaction);
    void setReactions(const std::vector<Reaction>& reactions);
    void updateDiscTypes(const std::map<int, DiscType>& updatedDiscTypes);
    void removeDiscTypes(const std::vector<DiscType>& discTypesToRemove);
    void clear();

private:
    void createLookupMaps();

private:
    std::vector<Reaction> reactions_;

    std::unordered_map<DiscType, std::vector<Reaction>> decompositionReactionLookupMap_;
    std::unordered_map<DiscType, std::vector<Reaction>> transformationReactionLookupMap_;
    std::unordered_map<std::pair<DiscType, DiscType>, std::vector<Reaction>> combinationReactionLookupMap_;
    std::unordered_map<std::pair<DiscType, DiscType>, std::vector<Reaction>> exchangeReactionLookupMap_;
};

#endif /* REACTIONTABLE_HPP */
