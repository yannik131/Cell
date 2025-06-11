#ifndef JSONSERIALIZERS_HPP
#define JSONSERIALIZERS_HPP

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <nlohmann/json.hpp>

namespace cell
{

void to_json(nlohmann::json& j, const sf::Vector2f& vector);
void from_json(nlohmann::json& j, sf::Vector2f& vector);

void to_json(nlohmann::json& j, const sf::Color& color);
void from_json(nlohmann::json& j, sf::Color& color);

void to_json(nlohmann::json& j, const sf::Time& time);
void from_json(const nlohmann::json& j, sf::Time& time);

} // namespace cell

#endif /* JSONSERIALIZERS_HPP */
