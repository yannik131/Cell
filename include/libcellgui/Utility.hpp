#ifndef UTILITY_HPP
#define UTILITY_HPP

#include "ComboBoxDelegate.hpp"
#include "DiscType.hpp"

namespace Utility
{

template <template <typename, typename> class MapType, typename KeyType, typename ValueType>
MapType<ValueType, KeyType> invertMap(const MapType<KeyType, ValueType>& map)
{
    MapType<ValueType, KeyType> inverted;

    for (auto it = map.begin(); it != map.end(); ++it)
        inverted[it.value()] = it.key();

    return inverted;
}

void setComboBoxItemsToDiscTypeNames(ComboBoxDelegate* comboBoxDelegate);

DiscType getDiscTypeByName(const QString& name);

}; // namespace Utility

#endif /* UTILITY_HPP */
