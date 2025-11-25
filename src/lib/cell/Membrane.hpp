#ifndef DD74492E_28F5_4688_960F_C9E90C40C167_HPP
#define DD74492E_28F5_4688_960F_C9E90C40C167_HPP

#include "MembraneType.hpp"
#include "PhysicalObject.hpp"
#include "Types.hpp"
#include "Vector2d.hpp"

namespace cell
{

class Compartment;

class Membrane : public PhysicalObject
{
public:
    explicit Membrane(const MembraneTypeID& membraneTypeID);

    MembraneTypeID getTypeID() const;

    void setCompartment(Compartment* compartment);
    Compartment* getCompartment() const;

private:
    MembraneTypeID membraneTypeID_;
    Compartment* compartment_ = nullptr;
};

} // namespace cell

#endif /* DD74492E_28F5_4688_960F_C9E90C40C167_HPP */
