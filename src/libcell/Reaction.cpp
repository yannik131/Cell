#include "Reaction.hpp"
#include "ExceptionWithLocation.hpp"

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
    }
}
} // namespace

bool operator==(const Reaction& reaction1, const Reaction& reaction2)
{
    return toString(reaction1) == toString(reaction2);
}

std::string toString(const Reaction& reaction)
{
    std::string result = reaction.getEduct1()->getName();

    if (reaction.hasEduct2())
        result += " + " + reaction.getEduct2()->getName();

    result += " -> " + reaction.getProduct1()->getName();

    if (reaction.hasProduct2())
        result += " + " + reaction.getProduct2()->getName();

    return result;
}

bool contains(const Reaction& reaction, const DiscType* discType)
{
    return reaction.getEduct1() == discType || reaction.getEduct2() == discType || reaction.getProduct1() == discType ||
           reaction.getProduct2() == discType;
}

size_t ReactionHash::operator()(const Reaction& reaction) const
{
    static std::hash<std::string> stringHash;

    return stringHash(toString(reaction));
}

Reaction::Type inferType(const DiscType* educt2, const DiscType* product2)
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

Reaction::Reaction(const DiscType* educt1, const DiscType* educt2, const DiscType* product1, const DiscType* product2,
                   double probability)
    : educt1_(educt1)
    , educt2_(educt2)
    , product1_(product1)
    , product2_(product2)
    , type_(inferType(educt2, product2))
{
    if (!educt1 || !product1)
        throw ExceptionWithLocation("Reactions require educt1 and product1 to be set");

    setProbability(probability);
}

const DiscType* Reaction::getEduct1() const
{
    return educt1_;
}

void Reaction::setEduct1(const DiscType* educt1)
{
    educt1_ = educt1;
}

const DiscType* Reaction::getEduct2() const
{
    if (!educt2_)
        throw ExceptionWithLocation("Can't get educt2 for reaction of type" + toString(type_));

    return educt2_;
}

bool Reaction::hasEduct2() const
{
    return type_ & (Combination | Exchange);
}

void Reaction::setEduct2(const DiscType* educt2)
{
    if (!educt2_)
        throw ExceptionWithLocation("Can't set educt2 for reaction of type" + toString(type_));
    educt2_ = educt2;
}

const DiscType* Reaction::getProduct1() const
{
    return product1_;
}

void Reaction::setProduct1(const DiscType* product1)
{
    product1_ = product1;
}

const DiscType* Reaction::getProduct2() const
{
    if (!product2_)
        throw ExceptionWithLocation("Can't get product2 for reaction of type" + toString(type_));

    return product2_;
}

bool Reaction::hasProduct2() const
{
    return type_ & (Decomposition | Exchange);
}

void Reaction::setProduct2(const DiscType* product2)
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

void Reaction::validate() const
{
    const double eductMass = educt1_->getMass() + (educt2_ ? educt2_->getMass() : 0);
    const double productMass = product1_->getMass() + (product2_ ? product2_->getMass() : 0);

    if (eductMass != productMass)
        throw ExceptionWithLocation(toString(*this) + ": Product- and educt masses need to be identical");

    if (type_ == Transformation && educt1_ == product1_)
        throw ExceptionWithLocation(toString(*this) + ": Educt 1 and product 1 are identical");
}

} // namespace cell