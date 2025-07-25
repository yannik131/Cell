#include "Membrane.hpp"
#include "Disc.hpp"
#include "ExceptionWithLocation.hpp"
#include "MathUtils.hpp"

namespace cell
{

void Membrane::setType(const MembraneType* membraneType)
{
    if (membraneType == nullptr)
        throw ExceptionWithLocation("Trying to set membrane type to a nullptr");

    membraneType_ = membraneType;
}

const MembraneType* Membrane::getType() const
{
    return membraneType_;
}

bool Membrane::contains(const Disc& disc) const
{
    return mathutils::distance(disc.getPosition(), getPosition()) <
           membraneType_->getRadius() + disc.getType()->getRadius();
}

} // namespace cell