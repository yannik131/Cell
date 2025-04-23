#include "GlobalGUISettings.hpp"
#include "GlobalSettings.hpp"
#include "GlobalSettingsFunctor.hpp"
#include "Utility.hpp"

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
    loadDefaultDiscTypesPlotMap();

    connect(&GlobalSettingsFunctor::get(), &GlobalSettingsFunctor::discTypeDistributionChanged, this,
            &GlobalGUISettings::updateDiscTypesPlotMap);
}

void GlobalGUISettings::loadDefaultDiscTypesPlotMap()
{
    QMap<DiscType, bool> discTypesPlotMap;
    for (const auto& [discType, _] : GlobalSettings::getSettings().discTypeDistribution_)
        discTypesPlotMap[discType] = true;

    setDiscTypesPlotMap(discTypesPlotMap);
}

void GlobalGUISettings::setGuiFPS(int guiFPS)
{
    throwIfNotInRange(guiFPS, GUISettingsLimits::MinGuiFPS, GUISettingsLimits::MaxGuiFPS, "GUI FPS");
    guiSettings_.guiFPS_ = guiFPS;
}

void GlobalGUISettings::setPlotTimeInterval(const sf::Time& plotTimeInterval)
{
    throwIfNotInRange(plotTimeInterval, GUISettingsLimits::MinPlotTimeInterval, GUISettingsLimits::MaxPlotTimeInterval,
                      "Plot time interval");
    guiSettings_.plotTimeInterval_ = plotTimeInterval;

    emit plotResetRequired();
}

void GlobalGUISettings::setCurrentPlotCategory(const PlotCategory& plotCategory)
{
    guiSettings_.currentPlotCategory_ = plotCategory;

    emit plotResetRequired();
}

void GlobalGUISettings::setDiscTypesPlotMap(const QMap<DiscType, bool>& discTypesPlotMap)
{
    guiSettings_.discTypesPlotMap_ = discTypesPlotMap;

    emit plotResetRequired();
}

void GlobalGUISettings::setDiscTypesPlotMap(const QStringList& selectedDiscTypeNames)
{
    QVector<DiscType> activeDiscTypes;
    for (const auto& selectedDiscTypeName : selectedDiscTypeNames)
        activeDiscTypes.push_back(Utility::getDiscTypeByName(selectedDiscTypeName));

    bool hasTrueValue = false;
    QMap<DiscType, bool> discTypePlotMap;
    for (const auto& [discType, _] : GlobalSettings::getSettings().discTypeDistribution_)
    {
        auto iter = std::find(activeDiscTypes.begin(), activeDiscTypes.end(), discType);
        discTypePlotMap[discType] = iter != activeDiscTypes.end();
        if (!hasTrueValue && discTypePlotMap[discType])
            hasTrueValue = true;
    }

    if (!hasTrueValue)
        throw std::runtime_error("Plot selection can't be empty");

    guiSettings_.discTypesPlotMap_ = discTypePlotMap;

    emit plotResetRequired();
}

void GlobalGUISettings::setPlotSum(bool value)
{
    guiSettings_.plotSum_ = value;

    emit plotResetRequired();
}

void GlobalGUISettings::updateDiscTypesPlotMap()
{
    QMap<DiscType, bool> updatedDiscTypesPlotMap;
    for (const auto& [discType, _] : GlobalSettings::getSettings().discTypeDistribution_)
    {
        if (guiSettings_.discTypesPlotMap_.contains(discType))
            updatedDiscTypesPlotMap[discType] = guiSettings_.discTypesPlotMap_[discType];
        else
            updatedDiscTypesPlotMap[discType] = true;
    }

    setDiscTypesPlotMap(updatedDiscTypesPlotMap);
}
