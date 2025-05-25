#include "ReactionTable.hpp"
#include "Reaction.hpp"

const DiscType::map<std::vector<Reaction>>& ReactionTable::getDecompositionReactionLookupMap() const
{
    return decompositionReactionLookupMap_;
}

const DiscType::map<std::vector<Reaction>>& ReactionTable::getTransformationReactionLookupMap() const
{
    return transformationReactionLookupMap_;
}

const DiscType::pair_map<std::vector<Reaction>>& ReactionTable::getCombinationReactionLookupMap() const
{
    return combinationReactionLookupMap_;
}

const DiscType::pair_map<std::vector<Reaction>>& ReactionTable::getExchangeReactionLookupMap() const
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

void ReactionTable::updateDiscTypes(const DiscType::map<DiscType>& updatedDiscTypes)
{
    std::vector<Reaction> updatedReactions;
    for (const auto& reaction : reactions_)
    {
        std::vector<std::optional<DiscType>> reactionParts(
            {reaction.getEduct1(), reaction.hasEduct2() ? std::optional<DiscType>(reaction.getEduct2()) : std::nullopt,
             reaction.getProduct1(),
             reaction.hasProduct2() ? std::optional<DiscType>(reaction.getProduct2()) : std::nullopt});
        for (auto& part : reactionParts)
        {
            if (part.has_value() && updatedDiscTypes.contains(*part))
                *part = updatedDiscTypes.at(*part);
        }

        Reaction updatedReaction{*reactionParts[0], reactionParts[1], *reactionParts[2], reactionParts[3],
                                 reaction.getProbability()};
        updatedReaction.validate();

        updatedReactions.push_back(updatedReaction);
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
        if (reaction.getType() & (Reaction::Transformation | Reaction::Decomposition))
        {
            auto& lookupMap = reaction.getType() & Reaction::Transformation ? transformationReactionLookupMap_
                                                                            : decompositionReactionLookupMap_;
            addReactionToVector(lookupMap[reaction.getEduct1()], reaction);
        }
        else
        {
            auto& lookupMap =
                reaction.getType() & Reaction::Combination ? combinationReactionLookupMap_ : exchangeReactionLookupMap_;
            addReactionToVector(lookupMap[std::make_pair(reaction.getEduct1(), reaction.getEduct2())], reaction);
            if (reaction.getEduct1() != reaction.getEduct2())
                addReactionToVector(lookupMap[std::make_pair(reaction.getEduct2(), reaction.getEduct1())], reaction);
        }
    }
}
