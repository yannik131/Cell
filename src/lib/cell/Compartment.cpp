#include "Compartment.hpp"
#include "Disc.hpp"

namespace cell
{

Compartment::Compartment(Membrane&& membrane)
    : membrane_(std::move(membrane))
{
}

Compartment::~Compartment() = default;

const Membrane& Compartment::getMembrane() const
{
    return membrane_;
}

} // namespace cell