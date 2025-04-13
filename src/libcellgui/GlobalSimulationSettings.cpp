#include "GlobalSimulationSettings.hpp"

GlobalSimulationSettings& GlobalSimulationSettings::get()
{
    static GlobalSimulationSettings instance;

    return instance;
}

const Settings& GlobalSimulationSettings::getSettings()
{
    return GlobalSimulationSettings::get().settings_;
}

void GlobalSimulationSettings::afterSettingsChanged(const SettingID& settingID)
{
    if (settingID & (SettingID::DiscTypeDistribution | SettingID::NumberOfDiscs))
        emit simulationResetRequired();

    if (settingID & SettingID::DiscTypeDistribution)
        emit discTypeDistributionChanged();
}
