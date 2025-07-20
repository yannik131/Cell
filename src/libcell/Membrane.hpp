#ifndef MEMBRANE_HPP
#define MEMBRANE_HPP

#include "PhysicalObject.hpp"
#include "types/MembraneType.hpp"

namespace cell
{
class Membrane : public PhysicalObject
{
public:
    void setType(const MembraneType* membraneType);

    const MembraneType* getType() const;

private:
    const MembraneType* membraneType_;
};

} // namespace cell

#endif /* MEMBRANE_HPP */
