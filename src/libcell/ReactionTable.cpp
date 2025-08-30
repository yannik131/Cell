#include "ReactionTable.hpp"
#include "ExceptionWithLocation.hpp"
#include "Reaction.hpp"

#include <algorithm>

namespace cell
{

ReactionTable::ReactionTable(DiscTypeResolver discTypeResolver)
    : discTypeResolver_(std::move(discTypeResolver))
{
}

const DiscTypeMap<std::vector<Reaction>>& ReactionTable::getTransformationReactionLookupMap() const
{
    return transformationReactionLookupMap_;
}

const DiscTypeMap<std::vector<Reaction>>& ReactionTable::getDecompositionReactionLookupMap() const
{
    return decompositionReactionLookupMap_;
}

const DiscTypePairMap<std::vector<Reaction>>& ReactionTable::getCombinationReactionLookupMap() const
{
    return combinationReactionLookupMap_;
}

const DiscTypePairMap<std::vector<Reaction>>& ReactionTable::getExchangeReactionLookupMap() const
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

void ReactionTable::removeDiscType(DiscTypeID discTypeToRemove)
{
    std::vector<Reaction> remainingReactions;
    for (const auto& reaction : reactions_)
    {
        if (reaction.getEduct1() == discTypeToRemove ||
            (reaction.hasEduct2() && reaction.getEduct2() == discTypeToRemove) ||
            reaction.getProduct1() == discTypeToRemove ||
            (reaction.hasProduct2() && reaction.getProduct2() == discTypeToRemove))
            continue;

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

const std::vector<Reaction>& ReactionTable::getReactions() const
{
    return reactions_;
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
            lookupMap[reaction.getEduct1()].push_back(reaction);
        }
        else
        {
            auto& lookupMap =
                reaction.getType() & Reaction::Combination ? combinationReactionLookupMap_ : exchangeReactionLookupMap_;

            lookupMap[std::make_pair(reaction.getEduct1(), reaction.getEduct2())].push_back(reaction);
            if (reaction.getEduct1() != reaction.getEduct2())
                lookupMap[std::make_pair(reaction.getEduct2(), reaction.getEduct1())].push_back(reaction);
        }
    }
}

} // namespace cell