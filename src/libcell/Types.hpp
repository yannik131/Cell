#ifndef TYPES_HPP
#define TYPES_HPP

#include "Hashing.hpp"

#include <cstdint>
#include <functional>
#include <unordered_map>
#include <utility>

namespace cell
{

class DiscType;
class Reaction;

using DiscTypeID = std::uint16_t;
using DiscTypeResolver = std::function<const DiscType&(DiscTypeID)>;

/**
 * @brief Type to be used for reaction and disc type distribution tables
 */
template <typename T> using DiscTypeMap = std::unordered_map<DiscTypeID, T>;

/**
 * @brief Type for the bimolecular reaction tables
 */
template <typename T> using DiscTypePairMap = std::unordered_map<std::pair<DiscTypeID, DiscTypeID>, T, PairHasher>;

} // namespace cell

#endif /* TYPES_HPP */
