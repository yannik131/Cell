#include "ColorMapping.hpp"
#include "Utility.hpp"

namespace sf
{
bool operator<(const sf::Color& a, const sf::Color& b)
{
    return std::tie(a.r, a.g, a.b) < std::tie(b.r, b.g, b.b);
}
} // namespace sf

/**
 * @brief Avoiding SIOF here
 */
const QMap<sf::Color, QString>& getColorNameMapping()
{
    static const QMap<sf::Color, QString> colorNameMapping = {
        {sf::Color::White, "White"}, {sf::Color::Red, "Red"},       {sf::Color::Green, "Green"},
        {sf::Color::Blue, "Blue"},   {sf::Color::Yellow, "Yellow"}, {sf::Color::Magenta, "Magenta"},
        {sf::Color::Cyan, "Cyan"}};
    return colorNameMapping;
}

const QMap<QString, sf::Color>& getNameColorMapping()
{
    static const QMap<QString, sf::Color> nameColorMapping = Utility::invertMap(getColorNameMapping());
    return nameColorMapping;
}

const QList<sf::Color>& getSupportedDiscColors()
{
    static const QList<sf::Color> supportedDiscColors = getColorNameMapping().keys();
    return supportedDiscColors;
}

const QStringList& getSupportedDiscColorNames()
{
    static const QStringList supportedDiscColorNames = getColorNameMapping().values();
    return supportedDiscColorNames;
}