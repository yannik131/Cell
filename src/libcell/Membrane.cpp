#include "Membrane.hpp"
#include "ExceptionWithLocation.hpp"

namespace cell
{

void Membrane::setType(const MembraneType* membraneType)
{
    if (membraneType == nullptr)
        throw ExceptionWithLocation("Trying to set membrane type to a nullptr");

    membraneType_ = membraneType;
}

const MembraneType& Membrane::getType() const
{
    return *membraneType_;
}

} // namespace cell