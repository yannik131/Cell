#ifndef COLORMAPPING_HPP
#define COLORMAPPING_HPP

#include <QList>
#include <QMap>
#include <SFML/Graphics/Color.hpp>

template <template <typename, typename> class MapType, typename KeyType, typename ValueType>
MapType<ValueType, KeyType> invertMap(const MapType<KeyType, ValueType>& map)
{
    MapType<ValueType, KeyType> inverted;

    for (auto it = map.begin(); it != map.end(); ++it)
        inverted[it.value()] = it.key();

    return inverted;
}

namespace sf
{
bool operator<(const sf::Color& a, const sf::Color& b);
} // namespace sf

extern const QMap<sf::Color, QString> ColorNameMapping;
extern const QMap<QString, sf::Color> NameColorMapping;

extern const QList<sf::Color> SupportedDiscColors;
extern const QStringList SupportedDiscColorNames;

#endif /* COLORMAPPING_HPP */
