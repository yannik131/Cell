#include "Reaction.hpp"
#include "ExceptionWithLocation.hpp"

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
    return reaction.getEduct1() == discType || (reaction.hasEduct2() && reaction.getEduct2() == discType) ||
           reaction.getProduct1() == discType || (reaction.hasProduct2() && reaction.getProduct2() == discType);
}

void addReactionToVector(std::vector<Reaction>& reactions, Reaction reaction)
{
    if (reactions.empty())
    {
        reactions.push_back(reaction);
        return;
    }

    if (std::find(reactions.begin(), reactions.end(), reaction) != reactions.end())
        throw ExceptionWithLocation("Duplicate reaction \"" + toString(reaction) + "\" not allowed");

    float totalProbability = reactions.front().getProbability();

    for (std::size_t i = 0; i < reactions.size() - 1; ++i)
        totalProbability += reactions[i + 1].getProbability() - reactions[i].getProbability();

    if (reaction.getProbability() + totalProbability > 1.f)
        throw ExceptionWithLocation("Can't add reaction to vector: Accumulative probability > 1");

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

Reaction::Type inferType(const std::optional<DiscType>& educt2, const std::optional<DiscType>& product2)
{
    if (!educt2.has_value() && !product2.has_value())
        return Reaction::Type::Transformation;
    else if (educt2.has_value() && !product2.has_value())
        return Reaction::Type::Combination;
    else if (!educt2.has_value() && product2.has_value())
        return Reaction::Type::Decomposition;
    else
        return Reaction::Type::Exchange;
}

Reaction::Reaction(const DiscType& educt1, const std::optional<DiscType>& educt2, const DiscType& product1,
                   const std::optional<DiscType>& product2, float probability)
    : educt1_(educt1)
    , educt2_(educt2)
    , product1_(product1)
    , product2_(product2)
    , type_(inferType(educt2, product2))
{
    setProbability(probability);
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
    if (type_ & (Transformation | Decomposition))
        throw ExceptionWithLocation("Can't get educt2: Decomposition/transformation reactions have no educt2");

    return *educt2_;
}

bool Reaction::hasEduct2() const
{
    return type_ & (Combination | Exchange);
}

void Reaction::setEduct2(const DiscType& educt2)
{
    if (type_ & (Transformation | Decomposition))
        throw ExceptionWithLocation("Can't set educt2: Decomposition reactions have no educt2");

    educt2_ = educt2;
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
    if (type_ & (Transformation | Combination))
        throw ExceptionWithLocation("Can't get product2: Combination/transformation reactions have no product2");

    return *product2_;
}

bool Reaction::hasProduct2() const
{
    return type_ & (Decomposition | Exchange);
}

void Reaction::setProduct2(const DiscType& product2)
{
    if (type_ & (Transformation | Combination))
        throw ExceptionWithLocation("Can't set product2: Combination/transformation reactions have no product2");

    product2_ = product2;
}

float Reaction::getProbability() const
{
    return probability_;
}

void Reaction::setProbability(float probability)
{
    if (probability < 0 || probability > 1)
        throw ExceptionWithLocation("Probability must be between 0 and 1");

    probability_ = probability;
}

const Reaction::Type& Reaction::getType() const
{
    return type_;
}

void Reaction::validate() const
{
    const float eductMass = educt1_.getMass() + (educt2_.has_value() ? educt2_->getMass() : 0);
    const float productMass = product1_.getMass() + (product2_.has_value() ? product2_->getMass() : 0);

    if (eductMass != productMass)
        throw ExceptionWithLocation(toString(*this) + ": Product- and educt masses need to be identical");

    if (type_ == Transformation && educt1_ == product1_)
        throw ExceptionWithLocation(toString(*this) + ": Educt 1 and product 1 are identical");
}
