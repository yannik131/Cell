#ifndef MEMBRANETYPE_HPP
#define MEMBRANETYPE_HPP

#include "DiscType.hpp"
#include "TypeBase.hpp"

#include <unordered_map>

namespace cell
{
class MembraneType : public TypeBase
{
public:
    enum PermeabilityType
    {
        PermeableInward,
        PermeableOutward,
        PermeableBidirectional,
        NotPermeable
    };

    template <typename T> using map = std::unordered_map<MembraneType*, T>;

public:
    MembraneType(const MembraneType&) = delete;

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

#endif /* MEMBRANETYPE_HPP */
