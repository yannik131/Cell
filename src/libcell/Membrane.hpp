#ifndef MEMBRANE_HPP
#define MEMBRANE_HPP

#include "MembraneType.hpp"
#include "PhysicalObject.hpp"

namespace cell
{
class Membrane : public PhysicalObject
{
public:
private:
    MembraneType membraneType_;
};

} // namespace cell

#endif /* MEMBRANE_HPP */
