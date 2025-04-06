#ifndef COLORMAPPING_HPP
#define COLORMAPPING_HPP

#include <QList>
#include <QMap>
#include <SFML/Graphics/Color.hpp>

namespace sf
{
bool operator<(const sf::Color& a, const sf::Color& b);
} // namespace sf

extern const QMap<sf::Color, QString> ColorNameMapping;
extern const QMap<QString, sf::Color> NameColorMapping;

extern const QList<sf::Color> SupportedDiscColors;
extern const QStringList SupportedDiscColorNames;

#endif /* COLORMAPPING_HPP */
