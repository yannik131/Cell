#include "Reaction.hpp"

#include <algorithm>
#include <functional>
#include <stdexcept>

bool operator==(const Reaction& reaction1, const Reaction& reaction2)
{
    return toString(reaction1) == toString(reaction2);
}

std::string toString(const Reaction& reaction)
{
    std::string result = reaction.getEduct1().getName();

    if (reaction.hasEduct2())
        result += " + " + reaction.getEduct2().getName();

    result += " -> " + reaction.getProduct1().getName();

    if (reaction.hasProduct2())
        result += " + " + reaction.getProduct2().getName();

    return result;
}

bool contains(const Reaction& reaction, const DiscType& discType)
{
    return reaction.getEduct1() == discType || reaction.getProduct1() == discType ||
           (reaction.hasEduct2() && reaction.getEduct2() == discType) ||
           (reaction.hasProduct2() && reaction.getProduct2() == discType);
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

    float totalProbability = reactions.front().getProbability();

    for (int i = 0; i < reactions.size() - 1; ++i)
        totalProbability += reactions[i + 1].getProbability() - reactions[i].getProbability();

    if (reaction.getProbability() + totalProbability > 1.f)
        throw std::runtime_error("Can't add reaction to vector: Accumulative probability > 1");

    reaction.setProbability(reaction.getProbability() + totalProbability);
    reactions.push_back(reaction);

    std::sort(reactions.begin(), reactions.end(), [](const auto& reaction1, const auto& reaction2)
              { return reaction1.getProbability() < reaction2.getProbability(); });
}

void removeReactionFromVector(std::vector<Reaction>& reactions, Reaction reaction)
{
    float subtrahend = 0;
    for (auto iter = reactions.begin(); iter != reactions.end();)
    {
        if (*iter == reaction)
        {
            if (iter == reactions.begin())
                subtrahend = iter->getProbability();
            else
                subtrahend = iter->getProbability() - (iter - 1)->getProbability();

            iter = reactions.erase(iter);
            continue;
        }

        if (subtrahend != 0)
            iter->setProbability(iter->getProbability() - subtrahend);

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

Reaction::Reaction(const DiscType& educt1, const std::optional<DiscType>& educt2, const DiscType& product1,
                   const std::optional<DiscType>& product2, const Type& type)
    : educt1_(educt1)
    , educt2_(educt2)
    , product1_(product1)
    , product2_(product2)
    , type_(type)
{
    switch (type_)
    {
    case Decomposition:
        if (educt2_.has_value() || !product2.has_value())
            throw std::runtime_error("Decomposition reactions can't have educt2 but require product2");
        break;
    case Combination:
        if (!educt2_.has_value() || product2_.has_value())
            throw std::runtime_error("Combination reactions require educt2 but can't have product2");
        break;
    case Exchange:
        if (!educt2_.has_value() || !product2_.has_value())
            throw std::runtime_error("Exchange reactions require both educt2 and product2");
        break;
    }
}

const DiscType& Reaction::getEduct1() const
{
    return educt1_;
}

void Reaction::setEduct1(const DiscType& educt1)
{
    educt1_ = educt1;
}

const DiscType& Reaction::getEduct2() const
{
    if (type_ == Decomposition)
        throw std::runtime_error("Can't get educt2: Decomposition reactions have no educt2");

    return *educt2_;
}

bool Reaction::hasEduct2() const
{
    return type_ != Decomposition;
}

void Reaction::setEduct2(const DiscType& educt2)
{
    if (type_ == Decomposition)
        throw std::runtime_error("Can't set educt2: Decomposition reactions have no educt2")
}

const DiscType& Reaction::getProduct1() const
{
    return product1_;
}

void Reaction::setProduct1(const DiscType& product1)
{
    product1_ = product1;
}

const DiscType& Reaction::getProduct2() const
{
    if (type_ == Combination)
        throw std::runtime_error("Can't get product2: Combination reactions have no product2");

    return *product2_;
}

bool Reaction::hasProduct2() const
{
    return type_ != Combination;
}

void Reaction::setProduct2(const DiscType& product2)
{
    if (type_ == Combination)
        throw std::runtime_error("Can't set product2: Combination reactions have no product2");

    product2_ = product2;
}

float Reaction::getProbability() const
{
    return probability_;
}

void Reaction::setProbability(float probability)
{
    if (probability < 0 || probability > 1)
        throw std::runtime_error("Probability must be between 0 and 1");

    probability_ = probability;
}

const Reaction::Type& Reaction::getType() const
{
    return type_;
}
