#include "Disc.hpp"
#include "ExceptionWithLocation.hpp"

#include <cmath>
#include <functional>
#include <stdexcept>

namespace cell
{

Disc::Disc(const DiscType& discType)
{
    setType(discType);
}

void Disc::setType(const DiscType& discType)
{
    type_ = discType;
    setMass(discType.getMass());
}

void Disc::markDestroyed()
{
    destroyed_ = true;
}

const DiscType& Disc::getType() const
{
    return type_;
}

bool Disc::isMarkedDestroyed() const
{
    return destroyed_;
}

} // namespace cell
