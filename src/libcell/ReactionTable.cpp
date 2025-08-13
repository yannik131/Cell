#include "ReactionTable.hpp"
#include "ExceptionWithLocation.hpp"
#include "Reaction.hpp"

#include <algorithm>

namespace cell
{

void addReactionToVector(std::vector<Reaction>& reactions, Reaction reaction)
{
    if (reactions.empty())
    {
        reactions.push_back(reaction);
        return;
    }

    for (const auto& r : reactions)
    {
        if (r == reaction)
            throw ExceptionWithLocation("Duplicate reaction \"" + toString(reaction) + "\" not allowed");

        if (r.getType() != reaction.getType())
            throw ExceptionWithLocation("Inconsistent reaction types: " + toString(r) + " vs. " + toString(reaction));
    }

    double totalProbability = reactions.front().getProbability();

    for (std::size_t i = 0; i < reactions.size() - 1; ++i)
        totalProbability += reactions[i + 1].getProbability() - reactions[i].getProbability();

    if (reaction.getProbability() + totalProbability > 1.0)
        throw ExceptionWithLocation("Can't add reaction to vector: Accumulative probability > 1");

    reaction.setProbability(reaction.getProbability() + totalProbability);
    reactions.push_back(reaction);

    std::ranges::sort(reactions, [](const auto& reaction1, const auto& reaction2)
                      { return reaction1.getProbability() < reaction2.getProbability(); });
}

ReactionTable::ReactionTable() = default;

const DiscType::map<std::vector<Reaction>>& ReactionTable::getTransformationReactionLookupMap() const
{
    return transformationReactionLookupMap_;
}

const DiscType::map<std::vector<Reaction>>& ReactionTable::getDecompositionReactionLookupMap() const
{
    return decompositionReactionLookupMap_;
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

void ReactionTable::removeDiscTypes(const std::vector<const DiscType*>& discTypesToRemove)
{
    std::vector<Reaction> remainingReactions;
    for (const auto& reaction : reactions_)
    {
        bool affected = false;
        for (const auto& removedDiscType : discTypesToRemove)
        {
            if (reaction.getEduct1() == removedDiscType ||
                (reaction.hasEduct2() && reaction.getEduct2() == removedDiscType) ||
                reaction.getProduct1() == removedDiscType ||
                (reaction.hasProduct2() && reaction.getProduct2() == removedDiscType))
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

} // namespace cell