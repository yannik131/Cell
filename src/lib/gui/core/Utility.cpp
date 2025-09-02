#include "core/Utility.hpp"
#include "cell/ExceptionWithLocation.hpp"

#include <stdexcept>

namespace utility
{

QColor sfColorToQColor(const sf::Color& sfColor)
{
    return {sfColor.r, sfColor.g, sfColor.b, sfColor.a};
}

} // namespace utility
