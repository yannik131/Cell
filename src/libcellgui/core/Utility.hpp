#ifndef UTILITY_HPP
#define UTILITY_HPP

#include "ComboBoxDelegate.hpp"
#include "DiscType.hpp"

template <typename T> QMap<DiscType, T>& operator+=(QMap<DiscType, T>& a, const QMap<DiscType, T>& b)
{
    for (auto iter = b.begin(); iter != b.end(); ++iter)
        a[iter.key()] += iter.value();

    return a;
}

template <typename T, typename DivisorType> QMap<DiscType, T>& operator/=(QMap<DiscType, T>& a, const DivisorType& b)
{
    for (auto iter = a.begin(); iter != a.end(); ++iter)
        iter.value() /= b;

    return a;
}

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

template <typename T1, typename T2, typename T3 = T2> QMap<T1, T2> convertToQMap(const std::map<T1, T3>& stdMap)
{
    QMap<T1, T2> qMap;

    for (const auto& pair : stdMap)
        qMap.insert(pair.first, static_cast<T2>(pair.second));

    return qMap;
}

DiscType getDiscTypeByName(const QString& name);
QStringList getDiscTypeNames();

QColor sfColorToQColor(const sf::Color& sfColor);

}; // namespace Utility

#endif /* UTILITY_HPP */
