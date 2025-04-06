#include "ColorMapping.hpp"
#include "Utility.hpp"

namespace sf
{
bool operator<(const sf::Color& a, const sf::Color& b)
{
    return std::tie(a.r, a.g, a.b) < std::tie(b.r, b.g, b.b);
}
} // namespace sf

const QMap<sf::Color, QString> ColorNameMapping = {{sf::Color::White, "White"},   {sf::Color::Red, "Red"},
                                                   {sf::Color::Green, "Green"},   {sf::Color::Blue, "Blue"},
                                                   {sf::Color::Yellow, "Yellow"}, {sf::Color::Magenta, "Magenta"},
                                                   {sf::Color::Cyan, "Cyan"}};
const QMap<QString, sf::Color> NameColorMapping = Utility::invertMap(ColorNameMapping);

const QList<sf::Color> SupportedDiscColors = ColorNameMapping.keys();
const QStringList SupportedDiscColorNames = ColorNameMapping.values();