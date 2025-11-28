#include "Disc.hpp"
#include "ExceptionWithLocation.hpp"
#include "Types.hpp"

namespace cell
{

Disc::Disc(DiscTypeID discTypeID)
    : discTypeID_(discTypeID)
{
}

void Disc::setType(DiscTypeID discTypeID)
{
    discTypeID_ = discTypeID;
}

void Disc::markDestroyed()
{
    destroyed_ = true;
}

DiscTypeID Disc::getTypeID() const
{
    return discTypeID_;
}

bool Disc::isMarkedDestroyed() const
{
    return destroyed_;
}

} // namespace cell
