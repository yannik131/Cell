#ifndef B9666EA7_02B2_4471_96B7_C53D3696A5E2_HPP
#define B9666EA7_02B2_4471_96B7_C53D3696A5E2_HPP

#include <QList>
#include <QMap>
#include <SFML/Graphics/Color.hpp>

/**
 * @brief Here we introduce the colors used for the discs in the GUI and mapping for sf::Color <-> QString
 */

namespace sf
{

/**
 * @brief Element wise comparison of rgb values for 2 colors
 */
bool operator<(const sf::Color& a, const sf::Color& b);

} // namespace sf

/**
 * @returns `QMap` mapping SFML colors to human readable names
 */
const QMap<sf::Color, QString>& getColorNameMapping();

/**
 * @return `QMap` mapping human readable color names to SFML colors
 */
const QMap<QString, sf::Color>& getNameColorMapping();

/**
 * @returns Keys of the color mapping: These are all the colors supported in the GUI for disc types
 */
const QVector<sf::Color>& getSupportedDiscColors();

/**
 * @returns Human readable color names for the supported colors in the GUI in the same order as
 * `getSupportedDiscColors()`
 */
const QStringList& getSupportedDiscColorNames();

#endif /* B9666EA7_02B2_4471_96B7_C53D3696A5E2_HPP */
