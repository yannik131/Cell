#ifndef UTILITY_HPP
#define UTILITY_HPP

#include "delegates/ComboBoxDelegate.hpp"

#include <SFML/Graphics/Color.hpp>

namespace utility
{

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
