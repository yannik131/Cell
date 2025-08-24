#include "GlobalGUISettings.hpp"
#include "ExceptionWithLocation.hpp"
#include "GlobalSettings.hpp"
#include "GlobalSettingsFunctor.hpp"
#include "Utility.hpp"

#include <algorithm>

GlobalGUISettings& GlobalGUISettings::get()
{
    static GlobalGUISettings instance;

    return instance;
}

const GUISettings& GlobalGUISettings::getGUISettings()
{
    return get().guiSettings_;
}

GlobalGUISettings::GlobalGUISettings()
{
    connect(&GlobalSettingsFunctor::get(), &GlobalSettingsFunctor::discTypeDistributionChanged, this,
            &GlobalGUISettings::updateDiscTypesPlotMap);
}

void GlobalGUISettings::setGuiFPS(int guiFPS)
{
    cell::throwIfNotInRange(guiFPS, GUISettingsLimits::MinGuiFPS, GUISettingsLimits::MaxGuiFPS, "GUI FPS");
    guiSettings_.guiFPS_ = guiFPS;
}

void GlobalGUISettings::setPlotTimeInterval(const sf::Time& plotTimeInterval)
{
    cell::throwIfNotInRange(plotTimeInterval, GUISettingsLimits::MinPlotTimeInterval,
                            GUISettingsLimits::MaxPlotTimeInterval, "Plot time interval");
    guiSettings_.plotTimeInterval_ = plotTimeInterval;

    emit replotRequired();
}

void GlobalGUISettings::setCurrentPlotCategory(const PlotCategory& plotCategory)
{
    guiSettings_.currentPlotCategory_ = plotCategory;

    emit replotRequired();
}

void GlobalGUISettings::setDiscTypesPlotMap(const QStringList& selectedDiscTypeNames)
{
    if (cell::GlobalSettings::getSettings().discTypeDistribution_.empty() && selectedDiscTypeNames.empty())
        return;

    QVector<cell::DiscType> activeDiscTypes;
    for (const auto& selectedDiscTypeName : selectedDiscTypeNames)
        activeDiscTypes.push_back(utility::getDiscTypeByName(selectedDiscTypeName));

    bool hasTrueValue = false;
    cell::DiscTypeMap<bool> discTypePlotMap;
    for (const auto& [discType, _] : cell::GlobalSettings::getSettings().discTypeDistribution_)
    {
        auto iter = std::ranges::find(activeDiscTypes, discType);
        discTypePlotMap[discType] = iter != activeDiscTypes.end();
        if (!hasTrueValue && discTypePlotMap[discType])
            hasTrueValue = true;
    }

    if (!hasTrueValue)
        throw ExceptionWithLocation("Plot selection can't be empty");

    guiSettings_.discTypesPlotMap_ = discTypePlotMap;

    emit replotRequired();
}

void GlobalGUISettings::setPlotSum(bool value)
{
    guiSettings_.plotSum_ = value;

    emit replotRequired();
}

void GlobalGUISettings::updateDiscTypesPlotMap()
{
    cell::DiscTypeMap<bool> updatedDiscTypesPlotMap;
    for (const auto& [discType, _] : cell::GlobalSettings::getSettings().discTypeDistribution_)
    {
        if (guiSettings_.discTypesPlotMap_.contains(discType))
            updatedDiscTypesPlotMap[discType] = guiSettings_.discTypesPlotMap_[discType];
        else
            updatedDiscTypesPlotMap[discType] = true;
    }

    guiSettings_.discTypesPlotMap_ = updatedDiscTypesPlotMap;
    emit replotRequired();
    emit plotDataMapUpdated();
}
