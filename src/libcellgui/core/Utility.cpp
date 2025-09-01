#include "Utility.hpp"
#include "ExceptionWithLocation.hpp"

#include <stdexcept>

namespace utility
{

cell::DiscType getDiscTypeByName(const QString& name)
{
    // TODO

    throw ExceptionWithLocation(("No disc type found for name \"" + name + "\"").toStdString());
}

QStringList getDiscTypeNames()
{
    // TODO
    QStringList discTypeNames;

    return discTypeNames;
}

QColor sfColorToQColor(const sf::Color& sfColor)
{
    return {sfColor.r, sfColor.g, sfColor.b, sfColor.a};
}

} // namespace utility
