#ifndef MEMBRANE_HPP
#define MEMBRANE_HPP

#include "DiscType.hpp"

#include <unordered_map>

namespace cell
{
class Membrane
{
public:
    enum PermeabilityType
    {
        permeableInward,
        permeableOutward,
        permeableBidirectional,
        notPermeable
    };

    void setPermeability(const DiscType& discType, PermeabilityType permeabilityType);

    PermeabilityType getPermeability(const DiscType& discType);

private:
    std::unordered_map<DiscType, PermeabilityType> permeabilityMap_;
};
} // namespace cell

#endif /* MEMBRANE_HPP */
