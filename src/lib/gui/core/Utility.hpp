#ifndef UTILITY_HPP
#define UTILITY_HPP

#include "ComboBoxDelegate.hpp"
#include "DiscType.hpp"

namespace utility
{

/**
 * @brief Iterates all disc types in the current disc type distribution and returns the one with the given name or
 * throws if it can't be found
 */
cell::DiscType getDiscTypeByName(const QString& name);

/**
 * @returns a list of all disc type names in the current distribution
 */
QStringList getDiscTypeNames();

/**
 * @brief Converts a sfml color to a QColor
 */
QColor sfColorToQColor(const sf::Color& sfColor);

/**
 * @brief Turns a QMap<key, value> into a map<value, key>
 */
template <template <typename, typename> class MapType, typename KeyType, typename ValueType>
MapType<ValueType, KeyType> invertMap(const MapType<KeyType, ValueType>& map)
{
    MapType<ValueType, KeyType> inverted;

    for (auto it = map.begin(); it != map.end(); ++it)
        inverted[it.value()] = it.key();

    return inverted;
}

}; // namespace utility

#endif /* UTILITY_HPP */
