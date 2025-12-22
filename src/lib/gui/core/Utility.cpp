#include "core/Utility.hpp"

namespace utility
{

QColor sfColorToQColor(const sf::Color& sfColor)
{
    return {sfColor.r, sfColor.g, sfColor.b, sfColor.a};
}

sf::Vector2f toVector2f(const cell::Vector2d& v)
{
    return sf::Vector2f{static_cast<float>(v.x), static_cast<float>(v.y)};
}

} // namespace utility
