#include "Compartment.hpp"

namespace cell
{

Compartment::Compartment(Membrane&& membrane)
    : membrane_(std::move(membrane))
{
}

const Membrane& Compartment::getMembrane() const
{
    return membrane_;
}

} // namespace cell