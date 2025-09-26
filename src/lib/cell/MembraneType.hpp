#ifndef BF6AA0FA_191C_4C99_AE82_01625CBB6222_HPP
#define BF6AA0FA_191C_4C99_AE82_01625CBB6222_HPP

#include "Types.hpp"

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

private:
    std::unordered_map<DiscTypeID, Permeability> permeabilityMap_;
    double radius_;
};

} // namespace cell

#endif /* BF6AA0FA_191C_4C99_AE82_01625CBB6222_HPP */
