#ifndef UTILITY_HPP
#define UTILITY_HPP

#include "ComboBoxDelegate.hpp"
#include "DiscType.hpp"

namespace Utility
{

DiscType getDiscTypeByName(const QString& name);
QStringList getDiscTypeNames();

QColor sfColorToQColor(const sf::Color& sfColor);

}; // namespace Utility

#endif /* UTILITY_HPP */
