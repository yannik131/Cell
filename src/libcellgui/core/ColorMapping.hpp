#ifndef COLORMAPPING_HPP
#define COLORMAPPING_HPP

#include <QList>
#include <QMap>
#include <SFML/Graphics/Color.hpp>

namespace sf
{
bool operator<(const sf::Color& a, const sf::Color& b);
} // namespace sf

const QMap<sf::Color, QString>& getColorNameMapping();

const QMap<QString, sf::Color>& getNameColorMapping();

const QList<sf::Color>& getSupportedDiscColors();

const QStringList& getSupportedDiscColorNames();

#endif /* COLORMAPPING_HPP */
