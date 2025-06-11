#include "JsonSerializers.hpp"

namespace cell
{

void to_json(nlohmann::json& j, const sf::Vector2f& vector)
{
    j = std::make_tuple(vector.x, vector.y);
}

void from_json(nlohmann::json& j, sf::Vector2f& vector)
{
    std::tie(vector.x, vector.y) = j.get<std::tuple<float, float>>();
}

void to_json(nlohmann::json& j, const sf::Color& color)
{
    j = std::make_tuple(color.r, color.g, color.b, color.a);
}

void from_json(nlohmann::json& j, sf::Color& color)
{
    using sf::Uint8;
    std::tie(color.r, color.g, color.b, color.a) = j.get<std::tuple<Uint8, Uint8, Uint8, Uint8>>();
}

void to_json(nlohmann::json& j, const sf::Time& time)
{
    j = time.asMicroseconds();
}

void from_json(const nlohmann::json& j, sf::Time& time)
{
    time = sf::microseconds(j.get<long long>());
}

} // namespace cell