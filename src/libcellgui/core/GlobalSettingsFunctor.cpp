#include "GlobalSettingsFunctor.hpp"

GlobalSettingsFunctor& GlobalSettingsFunctor::get()
{
    static GlobalSettingsFunctor instance;

    return instance;
}

void GlobalSettingsFunctor::operator()(const SettingID& settingID)
{
    if (settingID & SettingID::NumberOfDiscs)
        emit numberOfDiscsChanged();

    if (settingID & SettingID::DiscTypeDistribution)
        emit discTypeDistributionChanged();
}