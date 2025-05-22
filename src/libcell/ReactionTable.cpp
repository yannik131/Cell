#include "ReactionTable.hpp"
#include "Reaction.hpp"

const std::unordered_map<DiscType, std::vector<Reaction>>& ReactionTable::getDecompositionReactionLookupMap() const
{
    return decompositionReactionLookupMap_;
}

const std::unordered_map<DiscType, std::vector<Reaction>>& ReactionTable::getTransformationReactionLookupMap() const
{
    return transformationReactionLookupMap_;
}

const std::unordered_map<std::pair<DiscType, DiscType>, std::vector<Reaction>>&
ReactionTable::getCombinationReactionLookupMap() const
{
    return combinationReactionLookupMap_;
}

const std::unordered_map<std::pair<DiscType, DiscType>, std::vector<Reaction>>&
ReactionTable::getExchangeReactionLookupMap() const
{
    return exchangeReactionLookupMap_;
}

void ReactionTable::addReaction(const Reaction& reaction)
{
    reactions_.push_back(reaction);
    createLookupMaps();
}

void ReactionTable::setReactions(const std::vector<Reaction>& reactions)
{
    reactions_ = reactions;
    createLookupMaps();
}

void ReactionTable::updateDiscTypes(const std::map<int, DiscType>& updatedDiscTypes)
{
    std::vector<Reaction> updatedReactions;
    for (const auto& reaction : reactions_)
    {
        std::vector<std::optional<DiscType>> reactionParts(
            {reaction.getEduct1(), reaction.getEduct2(), reaction.getProduct1(), reaction.getProduct2()});
        for (auto& part : reactionParts)
        {
            if (part.has_value() && updatedDiscTypes.contains(part->getId()))
                *part = updatedDiscTypes.at(part->getId());
        }

        updatedReactions.push_back(Reaction{*reactionParts[0], reactionParts[1], *reactionParts[2], reactionParts[3],
                                            reaction.getProbability()});
    }

    reactions_ = updatedReactions;
    createLookupMaps();
}

void ReactionTable::removeDiscTypes(const std::vector<DiscType>& discTypesToRemove)
{
    std::vector<Reaction> remainingReactions;
    for (const auto& reaction : reactions_)
    {
        bool affected = false;
        for (const auto& removedDiscType : discTypesToRemove)
        {
            if (reaction.getEduct1() == removedDiscType ||
                reaction.hasEduct2() && reaction.getEduct2() == removedDiscType ||
                reaction.getProduct1() == removedDiscType ||
                reaction.hasProduct2() && reaction.getProduct2() == removedDiscType)
            {
                affected = true;
                break;
            }
        }

        if (!affected)
            remainingReactions.push_back(reaction);
    }

    reactions_ = remainingReactions;
    createLookupMaps();
}

void ReactionTable::clear()
{
    reactions_.clear();
    createLookupMaps();
}

void ReactionTable::createLookupMaps()
{
    transformationReactionLookupMap_.clear();
    decompositionReactionLookupMap_.clear();
    combinationReactionLookupMap_.clear();
    exchangeReactionLookupMap_.clear();

    for (const auto& reaction : reactions_)
    {
        if (reaction.getType() == Reaction::Transformation || reaction.getType() == Reaction::Decomposition)
        {
            auto lookupMap = reaction.getType() == Reaction::Transformation ? transformationReactionLookupMap_
                                                                            : decompositionReactionLookupMap_;
            lookupMap[reaction.getEduct1()].push_back(reaction);
            addReactionToVector(lookupMap[reaction.getEduct1()], reaction);
        }
        else
        {
            auto lookupMap = reaction.getType() == Reaction::Combination ? combinationReactionLookupMap_
                                                                         : exchangeReactionLookupMap_;
            addReactionToVector(lookupMap[std::make_pair(reaction.getEduct1(), reaction.getEduct2())], reaction);
            if (reaction.getEduct1() != reaction.getEduct2())
                addReactionToVector(lookupMap[std::make_pair(reaction.getEduct2(), reaction.getEduct1())], reaction);
        }
    }
}
