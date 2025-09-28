#ifndef BF6AA0FA_191C_4C99_AE82_01625CBB6222_HPP
#define BF6AA0FA_191C_4C99_AE82_01625CBB6222_HPP

#include "Types.hpp"
#include "Vector2d.hpp"

namespace cell
{

class MembraneType
{
public:
    enum class Permeability
    {
        Inward = 1 << 0,
        Outward = 1 << 1,
        Bidirectional = 1 << 2,
        None = 1 << 3
    };

    using PermeabilityMap = std::unordered_map<DiscTypeID, Permeability>;

public:
    MembraneType(std::string name, double radius, PermeabilityMap permeabilityMap);

    MembraneType(const MembraneType&) = delete;
    MembraneType& operator=(const MembraneType&) = delete;
    MembraneType(MembraneType&&) = default;
    MembraneType& operator=(MembraneType&&) = default;

    Permeability getPermeabilityFor(const DiscTypeID& discTypeID) const;

    const std::string& getName() const;
    double getRadius() const;

private:
    std::string name_;
    PermeabilityMap permeabilityMap_;
    double radius_;
};

} // namespace cell

#endif /* BF6AA0FA_191C_4C99_AE82_01625CBB6222_HPP */
