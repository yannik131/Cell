#include "Membrane.hpp"

namespace cell
{

Membrane::Membrane(const MembraneTypeID& membraneTypeID)
    : membraneTypeID_(membraneTypeID)
{
}

MembraneTypeID Membrane::getTypeID() const
{
    return membraneTypeID_;
}

void Membrane::setCompartment(Compartment* compartment)
{
    compartment_ = compartment;
}

Compartment* Membrane::getCompartment() const
{
    return compartment_;
}

} // namespace cell