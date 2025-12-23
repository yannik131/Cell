#include "core/ColorMapping.hpp"
#include "core/Utility.hpp"

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
        // Default SFML colors
        {sf::Color::White, "White"},
        {sf::Color::Red, "Red"},
        {sf::Color::Green, "Green"},
        {sf::Color::Blue, "Blue"},
        {sf::Color::Yellow, "Yellow"},
        {sf::Color::Magenta, "Magenta"},
        {sf::Color::Cyan, "Cyan"},

        // +10 additional easily distinguishable colors
        {sf::Color(255, 165, 0), "Orange"},
        {sf::Color(128, 0, 128), "Purple"},
        {sf::Color(165, 42, 42), "Brown"},
        {sf::Color(150, 75, 0), "Chestnut"},
        {sf::Color(255, 192, 203), "Pink"},
        {sf::Color(128, 128, 128), "Gray"},
        {sf::Color(0, 128, 128), "Teal"},
        {sf::Color(0, 0, 128), "Navy"},
        {sf::Color(128, 0, 0), "Maroon"},
        {sf::Color(255, 215, 0), "Gold"},
    };
    return colorNameMapping;
}

const QMap<QString, sf::Color>& getNameColorMapping()
{
    static const auto nameColorMapping = utility::invertMap(getColorNameMapping());
    return nameColorMapping;
}

const QVector<sf::Color>& getSupportedDiscColors()
{
    static const QVector<sf::Color> supportedDiscColors = getColorNameMapping().keys();
    return supportedDiscColors;
}

const QStringList& getSupportedDiscColorNames()
{
    static const QStringList supportedDiscColorNames = getColorNameMapping().values();
    return supportedDiscColorNames;
}
