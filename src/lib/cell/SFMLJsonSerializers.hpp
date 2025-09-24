#ifndef B74265AF_DA38_444B_B26F_28D40D103D68_HPP
#define B74265AF_DA38_444B_B26F_28D40D103D68_HPP

#include "DiscType.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <nlohmann/json.hpp>

// See https://github.com/nlohmann/json?tab=readme-ov-file#how-do-i-convert-third-party-types for more info on what's
// happening here and everywhere else where argument dependent lookup (adl) serializers are used

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

template <> struct adl_serializer<sf::Time>
{
    static void to_json(json& j, const sf::Time& t)
    {
        j = t.asMicroseconds();
    }

    static void from_json(const json& j, sf::Time& t)
    {
        t = sf::microseconds(j.get<long long>());
    }
};

} // namespace nlohmann

#endif /* B74265AF_DA38_444B_B26F_28D40D103D68_HPP */
