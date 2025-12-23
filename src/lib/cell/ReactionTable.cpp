#include "ReactionTable.hpp"
#include "ExceptionWithLocation.hpp"
#include "Reaction.hpp"

#include <algorithm>

namespace cell
{

ReactionTable::ReactionTable(const DiscTypeRegistry& discTypeRegistry)
    : discTypeRegistry_(discTypeRegistry)
{
}

const DiscTypeMap<std::vector<Reaction>>& ReactionTable::getTransformations() const
{
    return transformations_;
}

const DiscTypeMap<std::vector<Reaction>>& ReactionTable::getDecompositions() const
{
    return decompositions_;
}

const DiscTypePairMap<std::vector<Reaction>>& ReactionTable::getCombinations() const
{
    return combinations_;
}

const DiscTypePairMap<std::vector<Reaction>>& ReactionTable::getExchanges() const
{
    return exchanges_;
}

void ReactionTable::addReaction(const Reaction& reaction)
{
    reaction.validate(discTypeRegistry_);
    checkIfIsDuplicateReaction(reaction);

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
    transformations_.clear();
    decompositions_.clear();
    combinations_.clear();
    exchanges_.clear();

    for (const auto& reaction : reactions_)
    {
        if (isUnary(reaction))
        {
            auto& map = unaryMap(*this, reaction);
            map[reaction.getEduct1()].push_back(reaction);
        }
        else
        {
            auto& map = binaryMap(*this, reaction);
            map[std::minmax(reaction.getEduct1(), reaction.getEduct2())].push_back(reaction);
        }
    }
}

void ReactionTable::checkIfIsDuplicateReaction(const Reaction& reaction) const
{
    const std::vector<Reaction>* reactions = nullptr;
    if (isUnary(reaction))
    {
        const auto& map = unaryMap(*this, reaction);
        if (!map.contains(reaction.getEduct1()))
            return;
        reactions = &map.at(reaction.getEduct1());
    }
    else
    {
        const auto& map = binaryMap(*this, reaction);
        if (!map.contains(std::minmax(reaction.getEduct1(), reaction.getEduct2())))
            return;
        reactions = &map.at(std::minmax(reaction.getEduct1(), reaction.getEduct2()));
    }

    for (const auto& existingReaction : *reactions)
    {
        if (existingReaction == reaction)
            throw ExceptionWithLocation("Duplicate reaction \"" + toString(existingReaction, discTypeRegistry_) + "\"");
    }
}

bool ReactionTable::isUnary(const Reaction& r) const
{
    return r.getType() == Reaction::Type::Transformation || r.getType() == Reaction::Type::Decomposition;
}

} // namespace cell