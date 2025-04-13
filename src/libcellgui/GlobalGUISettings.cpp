#include "GlobalGUISettings.hpp"
#include "GlobalSettings.hpp"

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
    guiSettings_.discTypesPlotMap_.clear();

    QVector<DiscType> activeDiscTypes;
    for (const auto& selectedDiscTypeName : selectedDiscTypeNames)
        activeDiscTypes.push_back(GlobalSettings::getDiscTypeByName(selectedDiscTypeName.toStdString()));

    for (const auto& [discType, _] : GlobalSettings::getSettings().discTypeDistribution_)
    {
        auto iter = std::find(activeDiscTypes.begin(), activeDiscTypes.end(), discType);
        guiSettings_.discTypesPlotMap_[discType] = iter != activeDiscTypes.end();
    }

    emit plotResetRequired();
}
