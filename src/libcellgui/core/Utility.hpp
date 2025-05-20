#ifndef UTILITY_HPP
#define UTILITY_HPP

#include "ComboBoxDelegate.hpp"
#include "DiscType.hpp"

namespace Utility
{

DiscType getDiscTypeByName(const QString& name);
QStringList getDiscTypeNames();

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

}; // namespace Utility

#endif /* UTILITY_HPP */
