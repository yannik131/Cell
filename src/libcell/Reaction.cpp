#include "Reaction.hpp"

#include <algorithm>
#include <functional>
#include <stdexcept>

ReactionType inferReactionType(const Reaction& reaction)
{
    if (isValid(reaction.educt1_) && !isValid(reaction.educt2_) && isValid(reaction.product1_) &&
        isValid(reaction.product2_))
    {
        return ReactionType::Decomposition;
    }
    else if (isValid(reaction.educt1_) && isValid(reaction.educt2_) && isValid(reaction.product1_) &&
             !isValid(reaction.product2_))
    {
        return ReactionType::Combination;
    }
    else if (isValid(reaction.educt1_) && isValid(reaction.educt2_) && isValid(reaction.product1_) &&
             isValid(reaction.product2_))
    {
        return ReactionType::Exchange;
    }

    return ReactionType::Invalid;
}

bool operator==(const Reaction& reaction1, const Reaction& reaction2)
{
    return makeOrderedPair(reaction1.educt1_, reaction1.educt2_) ==
               makeOrderedPair(reaction2.educt1_, reaction2.educt2_) &&
           makeOrderedPair(reaction1.product1_, reaction1.product2_) ==
               makeOrderedPair(reaction2.product1_, reaction2.product2_) &&
           reaction1.probability_ == reaction2.probability_;
}

std::string toString(const Reaction& reaction)
{
    std::string result = reaction.educt1_.name_;

    if (!reaction.educt2_.name_.empty())
        result += " + " + reaction.educt2_.name_;

    result += " -> " + reaction.product1_.name_;

    if (!reaction.product2_.name_.empty())
        result += " + " + reaction.product2_.name_;

    return result;
}

bool contains(const Reaction& reaction, const DiscType& discType)
{
    return reaction.educt1_ == discType || reaction.educt2_ == discType || reaction.product1_ == discType ||
           reaction.product2_ == discType;
}

void addReactionToVector(std::vector<Reaction>& reactions, Reaction reaction)
{
    if (reactions.empty())
    {
        reactions.push_back(reaction);
        return;
    }

    if (std::find(reactions.begin(), reactions.end(), reaction) != reactions.end())
        throw std::runtime_error("Duplicate reaction \"" + toString(reaction) + "\" not allowed");

    float totalProbability = reactions.front().probability_;

    for (int i = 0; i < reactions.size() - 1; ++i)
        totalProbability += reactions[i + 1].probability_ - reactions[i].probability_;

    if (reaction.probability_ + totalProbability > 1.f)
        throw std::runtime_error("Can't add reaction to vector: Accumulative probability > 1");

    reaction.probability_ += totalProbability;
    reactions.push_back(reaction);

    std::sort(reactions.begin(), reactions.end(), [](const auto& reaction1, const auto& reaction2)
              { return reaction1.probability_ < reaction2.probability_; });
}

void removeReactionFromVector(std::vector<Reaction>& reactions, Reaction reaction)
{
    float subtrahend = 0;
    for (auto iter = reactions.begin(); iter != reactions.end();)
    {
        if (*iter == reaction)
        {
            if (iter == reactions.begin())
                subtrahend = iter->probability_;
            else
                subtrahend = iter->probability_ - (iter - 1)->probability_;

            iter = reactions.erase(iter);
            continue;
        }

        if (subtrahend != 0)
            iter->probability_ -= subtrahend;

        ++iter;
    }
}

void removeReactionsFromVector(std::vector<Reaction>& reactions, const DiscType& discType)
{
    std::vector<Reaction> reactionsToRemove;

    for (const auto& reaction : reactions)
    {
        if (contains(reaction, discType))
            reactionsToRemove.push_back(reaction);
    }

    if (reactions.size() == reactionsToRemove.size())
    {
        reactions.clear();
        return;
    }

    for (const auto& reaction : reactionsToRemove)
        removeReactionFromVector(reactions, reaction);
}

size_t ReactionHash::operator()(const Reaction& reaction) const
{
    static std::hash<std::string> stringHash;

    return stringHash(toString(reaction));
}
