#ifndef CD2404D2_405E_4617_8483_0D8737CD5D63_HPP
#define CD2404D2_405E_4617_8483_0D8737CD5D63_HPP

#include "Hashing.hpp"
#include "TypeRegistry.hpp"

#include <cstdint>
#include <functional>
#include <unordered_map>
#include <utility>

namespace cell
{

class DiscType;
class Reaction;

using DiscTypeID = std::uint16_t;
using MembraneTypeID = std::uint16_t;

using DiscTypeRegistry = TypeRegistry<DiscTypeID, DiscType>;
// using MembraneTypeRegistry = TypeRegistry<MembraneTypeID, MembraneType>;

/**
 * @brief Type to be used for reaction and disc type distribution tables
 */
template <typename T> using DiscTypeMap = std::unordered_map<DiscTypeID, T>;

/**
 * @brief Type for the bimolecular reaction tables
 */
template <typename T> using DiscTypePairMap = std::unordered_map<std::pair<DiscTypeID, DiscTypeID>, T, PairHasher>;

template <typename T> DiscTypeMap<T> operator+=(DiscTypeMap<T>& a, const DiscTypeMap<T>& b)
{
    for (const auto& [key, value] : b)
        a[key] += value;

    return a;
}

struct Radius
{
    double value;
};

struct Mass
{
    double value;
};

struct Position
{
    double x, y;
};

struct Velocity
{
    double x, y;
};

struct Dimensions
{
    double width, height;
};

struct Width
{
    double value;
};

struct Height
{
    double value;
};

struct Probability
{
    double value;
};

} // namespace cell

#endif /* CD2404D2_405E_4617_8483_0D8737CD5D63_HPP */
