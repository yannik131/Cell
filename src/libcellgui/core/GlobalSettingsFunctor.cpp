#include "GlobalSettingsFunctor.hpp"

GlobalSettingsFunctor& GlobalSettingsFunctor::get()
{
    static GlobalSettingsFunctor instance;

    return instance;
}

void GlobalSettingsFunctor::operator()(const cell::SettingID& settingID)
{
    if (settingID & cell::SettingID::NumberOfDiscs)
        emit numberOfDiscsChanged();

    if (settingID & cell::SettingID::DiscTypeDistribution)
        emit discTypeDistributionChanged();

    if (settingID & cell::SettingID::SimulationTimeScale)
        emit simulationTimeScaleChanged();

    if (settingID & cell::SettingID::SimulationTimeStep)
        emit simulationTimeStepChanged();
}
