#ifndef SFMLJSONSERIALIZER_HPP
#define SFMLJSONSERIALIZER_HPP

#include "DiscType.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <nlohmann/json.hpp>

namespace nlohmann
{

template <> struct adl_serializer<sf::Vector2f>
{
    static void to_json(json& j, const sf::Vector2f& v)
    {
        j = std::make_tuple(v.x, v.y);
    }

    static void from_json(const json& j, sf::Vector2f& v)
    {
        std::tie(v.x, v.y) = j.get<std::tuple<float, float>>();
    }
};

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

#endif
