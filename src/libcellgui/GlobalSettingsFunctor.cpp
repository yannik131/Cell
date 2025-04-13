#include "GlobalSettingsFunctor.hpp"

GlobalSettingsFunctor& GlobalSettingsFunctor::get()
{
    static GlobalSettingsFunctor instance;

    return instance;
}

void GlobalSettingsFunctor::operator()(const SettingID& settingID)
{
    if (settingID & (SettingID::DiscTypeDistribution | SettingID::NumberOfDiscs))
        emit simulationResetRequired();

    if (settingID & SettingID::DiscTypeDistribution)
        emit discTypeDistributionChanged();
}