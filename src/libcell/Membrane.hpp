#ifndef MEMBRANE_HPP
#define MEMBRANE_HPP

#include "DiscType.hpp"
#include "PhysicalObject.hpp"

#include <unordered_map>

namespace cell
{
class Membrane : public PhysicalObject
{
public:
    enum PermeabilityType
    {
        PermeableInward,
        PermeableOutward,
        PermeableBidirectional,
        NotPermeable
    };

    void setPermeability(const DiscType& discType, PermeabilityType permeabilityType);

    PermeabilityType getPermeability(const DiscType& discType);

    double getRadius() const;
    void setRadius(double radius);

    double getThickness() const;
    void setThickness(double thickness);

private:
    std::unordered_map<DiscType, PermeabilityType> permeabilityMap_;
    double radius_;
    double thickness_;
};
} // namespace cell

#endif /* MEMBRANE_HPP */
