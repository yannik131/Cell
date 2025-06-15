#include <SFML/Graphics/Color.hpp>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <optional>
#include <tuple>
#include <unordered_map>
#include <vector>

using json = nlohmann::json;

namespace nlohmann
{

template <> struct adl_serializer<sf::Color>
{
    static void to_json(json& j, const sf::Color& c)
    {
        j = std::tie(c.r, c.g, c.b, c.a);
    }

    static void from_json(const json& j, sf::Color& c)
    {
        using sf::Uint8;
        std::tie(c.r, c.g, c.b, c.a) = j.get<std::tuple<Uint8, Uint8, Uint8, Uint8>>();
    }
};

template <> struct adl_serializer<std::optional<sf::Color>>
{
    static void to_json(json& j, const std::optional<sf::Color>& c)
    {
        if (c.has_value())
            j = *c;
        else
            j = nullptr;
    }

    static void from_json(const json& j, std::optional<sf::Color>& c)
    {
        if (j.is_null())
            c = std::nullopt;
        else
            c = j.get<sf::Color>();
    }
};

} // namespace nlohmann

class OptionalColor
{
public:
    struct ColorHasher
    {
        std::size_t operator()(const OptionalColor& c) const
        {
            static std::hash<std::string> hasher;
            return hasher(c.getColorName());
        }
    };

    struct ColorEqualizer
    {
        bool operator()(const OptionalColor& a, const OptionalColor& b) const
        {
            return a.getColorName() == b.getColorName();
        }
    };

public:
    template <typename T> using map = std::unordered_map<OptionalColor, T, ColorHasher, ColorEqualizer>;

public:
    OptionalColor(const std::optional<sf::Color>& color, const std::string& colorName)
        : color_(color)
        , colorName_(colorName)
    {
    }

    const std::optional<sf::Color>& getColor() const
    {
        return color_;
    }
    const std::string& getColorName() const
    {
        return colorName_;
    }

private:
    std::optional<sf::Color> color_;
    std::string colorName_;
};

// The NLOHMANN_DEFINE_TYPE_ macros require a default-constructor. If the class doesn't have one, the following syntax
// can be used if the type is move-constructible, which it is since it has the compiler-generated move-constructor.
// Reminder: The compiler generates a move constructor unless one of these is defined in the class: destructor, copy
// constructor, copy assignment operator, move constructor
// See
// https://json.nlohmann.me/features/arbitrary_types/#how-can-i-use-get-for-non-default-constructiblenon-copyable-types
namespace nlohmann
{
template <> struct adl_serializer<OptionalColor>
{
    static OptionalColor from_json(const json& j)
    {
        return OptionalColor{j.at("color_").get<std::optional<sf::Color>>(), j.at("colorName_").get<std::string>()};
    }

    static void to_json(json& j, OptionalColor c)
    {
        j = json{{"color_", c.getColor()}, {"colorName_", c.getColorName()}};
    }
};
} // namespace nlohmann

std::ostream& operator<<(std::ostream& os, const OptionalColor& color)
{
    os << "Name: " << color.getColorName() << "\n";
    if (!color.getColor().has_value())
        os << "Contains no color.\n";
    else
    {
        sf::Color c = *color.getColor();
        os << "Red: " << static_cast<int>(c.r) << "\n";
        os << "Green: " << static_cast<int>(c.g) << "\n";
        os << "Blue: " << static_cast<int>(c.b) << "\n";
        os << "Alpha: " << static_cast<int>(c.a) << "\n";
    }

    return os << "\n";
}

int main()
{
    OptionalColor c{sf::Color::Red, "Red"}, d{sf::Color::Blue, "Blue"};
    OptionalColor::map<std::vector<OptionalColor>> theMap;

    std::vector<OptionalColor> colors(
        {OptionalColor{sf::Color::Green, "Green"}, OptionalColor{std::nullopt, "Nothing"}});

    theMap[c] = colors;
    theMap[d] = colors;

    {
        std::ofstream out("colors.json");
        json j = theMap;
        out << j.dump(4);
    }

    std::ifstream in("colors.json");
    json j;
    in >> j;

    auto colorsCopy = j.get<OptionalColor::map<std::vector<OptionalColor>>>();

    for (const auto& color : colorsCopy[c])
        std::cout << color;

    return 0;
}