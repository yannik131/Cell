#include "Utility.hpp"
#include "GlobalSettings.hpp"

#include <stdexcept>

namespace Utility
{

void setComboBoxItemsToDiscTypeNames(ComboBoxDelegate* comboBoxDelegate)
{
    QStringList discTypeNames;
    for (const auto& [discType, _] : GlobalSettings::getSettings().discTypeDistribution_)
        discTypeNames.push_back(QString::fromStdString(discType.getName()));

    comboBoxDelegate->setAvailableItems(std::move(discTypeNames));
}

DiscType getDiscTypeByName(const QString& name)
{
    for (const auto& [discType, frequency] : GlobalSettings::getSettings().discTypeDistribution_)
    {
        if (discType.getName() == name)
            return discType;
    }

    throw std::runtime_error(("No disc type found for name \"" + name + "\"").toStdString());
}

} // namespace Utility
