#include "core/ColorMapping.hpp"
#include "core/Utility.hpp"

namespace sf
{

bool operator<(const sf::Color& a, const sf::Color& b)
{
    return std::tie(a.r, a.g, a.b) < std::tie(b.r, b.g, b.b);
}

} // namespace sf

static QVector<std::pair<sf::Color, QString>> sortedColors()
{
    static const QVector<std::pair<sf::Color, QString>> sorted = {
        {sf::Color::Red, "Red"},           {sf::Color(0, 128, 128), "Teal"},   {sf::Color(255, 165, 0), "Orange"},
        {sf::Color(0, 0, 128), "Navy"},    {sf::Color::Yellow, "Yellow"},      {sf::Color(128, 0, 128), "Purple"},
        {sf::Color::Green, "Green"},       {sf::Color(255, 192, 203), "Pink"}, {sf::Color::Blue, "Blue"},
        {sf::Color(165, 42, 42), "Brown"}, {sf::Color::Cyan, "Cyan"},          {sf::Color(128, 0, 0), "Maroon"},
        {sf::Color(255, 215, 0), "Gold"},  {sf::Color(128, 128, 128), "Gray"}, {sf::Color(150, 75, 0), "Chestnut"},
        {sf::Color::Magenta, "Magenta"},   {sf::Color::White, "White"},
    };

    return sorted;
}

const QVector<sf::Color>& getSupportedDiscColors()
{
    static QVector<sf::Color> colors = []
    {
        QVector<sf::Color> v;
        for (const auto& p : sortedColors())
            v.push_back(p.first);
        return v;
    }();
    return colors;
}

const QVector<QString>& getSupportedDiscColorNames()
{
    static QVector<QString> names = []
    {
        QVector<QString> v;
        for (const auto& p : sortedColors())
            v.push_back(p.second);
        return v;
    }();
    return names;
}

const QMap<sf::Color, QString>& getColorNameMapping()
{
    static QMap<sf::Color, QString> map = []
    {
        QMap<sf::Color, QString> m;
        for (const auto& p : sortedColors())
            m.insert(p.first, p.second);
        return m;
    }();
    return map;
}

const QMap<QString, sf::Color>& getNameColorMapping()
{
    static QMap<QString, sf::Color> map = []
    {
        QMap<QString, sf::Color> m;
        for (const auto& p : sortedColors())
            m.insert(p.second, p.first);
        return m;
    }();
    return map;
}
