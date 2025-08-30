#include "Reaction.hpp"
#include "ExceptionWithLocation.hpp"
#include "Hashing.hpp"

namespace cell
{

namespace
{
std::string toString(Reaction::Type type)
{
    switch (type)
    {
    case Reaction::Decomposition:
        return "Decomposition";
    case Reaction::Transformation:
        return "Transformation";
    case Reaction::Combination:
        return "Combination";
    case Reaction::Exchange:
        return "Exchange";
    case Reaction::None:
        return "None";
    default:
        throw ExceptionWithLocation("Invalid reaction type");
    }
}

Reaction::Type inferType(const std::optional<DiscTypeID>& educt2, const std::optional<DiscTypeID>& product2)
{
    if (!educt2 && !product2)
        return Reaction::Type::Transformation;
    else if (educt2 && !product2)
        return Reaction::Type::Combination;
    else if (!educt2 && product2)
        return Reaction::Type::Decomposition;
    else
        return Reaction::Type::Exchange;
}
} // namespace

bool operator==(const Reaction& a, const Reaction& b)
{
    return a.getEduct1() == b.getEduct1() && a.hasEduct2() == b.hasEduct2() &&
           (!a.hasEduct2() || a.getEduct2() == b.getEduct2()) && a.getProduct1() == b.getProduct1() &&
           a.hasProduct2() == b.hasProduct2() && (!a.hasProduct2() || a.getProduct2() == b.getProduct2());
}

std::string toString(const Reaction& reaction, const DiscTypeResolver& discTypeResolver)
{
    std::string result = discTypeResolver(reaction.getEduct1()).getName();

    if (reaction.hasEduct2())
        result += " + " + discTypeResolver(reaction.getEduct2()).getName();

    result += " -> " + discTypeResolver(reaction.getProduct1()).getName();

    if (reaction.hasProduct2())
        result += " + " + discTypeResolver(reaction.getProduct2()).getName();

    return result;
}

bool contains(const Reaction& reaction, DiscTypeID discType)
{
    return reaction.getEduct1() == discType || reaction.getEduct2() == discType || reaction.getProduct1() == discType ||
           reaction.getProduct2() == discType;
}

size_t ReactionHash::operator()(const Reaction& reaction) const
{
    auto hash = calculateHash(reaction.getEduct1(), reaction.getProduct1());

    if (reaction.hasEduct2())
        hashCombine(hash, reaction.getEduct2());

    if (reaction.hasProduct2())
        hashCombine(hash, reaction.getProduct2());

    return hash;
}

Reaction::Reaction(DiscTypeID educt1, const std::optional<DiscTypeID>& educt2, DiscTypeID product1,
                   const std::optional<DiscTypeID>& product2, double probability)
    : educt1_(educt1)
    , educt2_(educt2)
    , product1_(product1)
    , product2_(product2)
    , type_(inferType(educt2, product2))
{
    setProbability(probability);
}

DiscTypeID Reaction::getEduct1() const
{
    return educt1_;
}

void Reaction::setEduct1(DiscTypeID educt1)
{
    educt1_ = educt1;
}

DiscTypeID Reaction::getEduct2() const
{
    if (!educt2_)
        throw ExceptionWithLocation("Can't get educt2 for reaction of type" + toString(type_));

    return *educt2_;
}

bool Reaction::hasEduct2() const
{
    return type_ & (Combination | Exchange);
}

void Reaction::setEduct2(DiscTypeID educt2)
{
    if (!educt2_)
        throw ExceptionWithLocation("Can't set educt2 for reaction of type" + toString(type_));
    educt2_ = educt2;
}

DiscTypeID Reaction::getProduct1() const
{
    return product1_;
}

void Reaction::setProduct1(DiscTypeID product1)
{
    product1_ = product1;
}

DiscTypeID Reaction::getProduct2() const
{
    if (!product2_)
        throw ExceptionWithLocation("Can't get product2 for reaction of type" + toString(type_));

    return *product2_;
}

bool Reaction::hasProduct2() const
{
    return type_ & (Decomposition | Exchange);
}

void Reaction::setProduct2(DiscTypeID product2)
{
    if (!product2_)
        throw ExceptionWithLocation("Can't set product2 for reaction of type" + toString(type_));

    product2_ = product2;
}

double Reaction::getProbability() const
{
    return probability_;
}

void Reaction::setProbability(double probability)
{
    if (probability < 0 || probability > 1)
        throw ExceptionWithLocation("Probability must be between 0 and 1");

    probability_ = probability;
}

const Reaction::Type& Reaction::getType() const
{
    return type_;
}

void Reaction::validate(const DiscTypeResolver& discTypeResolver) const
{
    auto getMass = [&](DiscTypeID discTypeID) { return discTypeResolver(discTypeID).getMass(); };

    const auto eductMassSum = getMass(educt1_) + (educt2_ ? getMass(*educt2_) : 0);
    const auto productMassSum = getMass(product1_) + (product2_ ? getMass(*product2_) : 0);

    if (eductMassSum != productMassSum)
        throw ExceptionWithLocation(toString(*this, discTypeResolver) +
                                    ": Product- and educt masses need to be identical");

    if (type_ == Transformation && educt1_ == product1_)
        throw ExceptionWithLocation(toString(*this, discTypeResolver) + ": Educt 1 and product 1 are identical");
}

} // namespace cell