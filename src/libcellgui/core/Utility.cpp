#include "Utility.hpp"
#include "ExceptionWithLocation.hpp"
#include "GlobalSettings.hpp"

#include <stdexcept>

namespace Utility
{

DiscType getDiscTypeByName(const QString& name)
{
    for (const auto& [discType, frequency] : GlobalSettings::getSettings().discTypeDistribution_)
    {
        if (discType.getName() == name)
            return discType;
    }

    throw ExceptionWithLocation(("No disc type found for name \"" + name + "\"").toStdString());
}

QStringList getDiscTypeNames()
{
    QStringList discTypeNames;
    for (const auto& [discType, _] : GlobalSettings::getSettings().discTypeDistribution_)
        discTypeNames.push_back(QString::fromStdString(discType.getName()));

    return discTypeNames;
}

QColor sfColorToQColor(const sf::Color& sfColor)
{
    return QColor(sfColor.r, sfColor.g, sfColor.b, sfColor.a);
}

} // namespace Utility
