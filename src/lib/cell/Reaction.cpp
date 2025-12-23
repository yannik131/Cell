#include "Reaction.hpp"
#include "ExceptionWithLocation.hpp"
#include "Hashing.hpp"

namespace cell
{

bool operator==(const Reaction& a, const Reaction& b)
{
    return a.getEduct1() == b.getEduct1() && a.hasEduct2() == b.hasEduct2() &&
           (!a.hasEduct2() || a.getEduct2() == b.getEduct2()) && a.getProduct1() == b.getProduct1() &&
           a.hasProduct2() == b.hasProduct2() && (!a.hasProduct2() || a.getProduct2() == b.getProduct2());
}

std::string toString(const Reaction& reaction, const DiscTypeRegistry& discTypeRegistry)
{
    std::string result = discTypeRegistry.getByID(reaction.getEduct1()).getName();

    if (reaction.hasEduct2())
        result += " + " + discTypeRegistry.getByID(reaction.getEduct2()).getName();

    result += " -> " + discTypeRegistry.getByID(reaction.getProduct1()).getName();

    if (reaction.hasProduct2())
        result += " + " + discTypeRegistry.getByID(reaction.getProduct2()).getName();

    return result;
}

bool contains(const Reaction& reaction, DiscTypeID discType)
{
    return reaction.getEduct1() == discType || reaction.getEduct2() == discType || reaction.getProduct1() == discType ||
           reaction.getProduct2() == discType;
}

Reaction::Type inferReactionType(bool educt2, bool product2)
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

Reaction::Reaction(DiscTypeID educt1, const std::optional<DiscTypeID>& educt2, DiscTypeID product1,
                   const std::optional<DiscTypeID>& product2, double probability)
    : educt1_(educt1)
    , educt2_(educt2)
    , product1_(product1)
    , product2_(product2)
    , type_(inferReactionType(educt2.has_value(), product2.has_value()))
{
    setProbability(probability);
}

void Reaction::setProbability(double probability)
{
    if (probability < 0 || probability > 1)
        throw ExceptionWithLocation("Probability must be between 0 and 1");

    probability_ = probability;
}

void Reaction::validate(const DiscTypeRegistry& discTypeRegistry) const
{
    auto getMass = [&](DiscTypeID discTypeID) { return discTypeRegistry.getByID(discTypeID).getMass(); };

    const auto eductMassSum = getMass(educt1_) + (educt2_ ? getMass(*educt2_) : 0);
    const auto productMassSum = getMass(product1_) + (product2_ ? getMass(*product2_) : 0);

    if (eductMassSum != productMassSum)
        throw ExceptionWithLocation(toString(*this, discTypeRegistry) +
                                    ": Product- and educt masses need to be identical");

    if (type_ == Type::Transformation && educt1_ == product1_)
        throw ExceptionWithLocation(toString(*this, discTypeRegistry) + ": Educt 1 and product 1 are identical");
}

std::string Reaction::getTypeString() const
{
    switch (type_)
    {
    case Reaction::Type::Decomposition: return "Decomposition";
    case Reaction::Type::Transformation: return "Transformation";
    case Reaction::Type::Combination: return "Combination";
    case Reaction::Type::Exchange: return "Exchange";
    case Reaction::Type::None: return "None";
    default: throw ExceptionWithLocation("Invalid reaction type");
    }
}

} // namespace cell