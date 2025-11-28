#ifndef TYPES_HPP
#define TYPES_HPP

#include "Hashing.hpp"
#include "TypeRegistry.hpp"

#include <cstdint>
#include <functional>
#include <unordered_map>
#include <utility>

namespace cell
{

class DiscType;
class MembraneType;
class Reaction;
class Disc;
class Membrane;

using DiscTypeID = TypeRegistry<Disc>::KeyType;
using MembraneTypeID = TypeRegistry<Membrane>::KeyType;

using DiscTypeRegistry = TypeRegistry<DiscType>;
using MembraneTypeRegistry = TypeRegistry<MembraneType>;

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

struct MinOverlap
{
    double value;
};

} // namespace cell

#endif /* TYPES_HPP */
