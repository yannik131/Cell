#ifndef GLOBALSIMULATIONSETTINGS_HPP
#define GLOBALSIMULATIONSETTINGS_HPP

#include "GlobalSettings.hpp"

#include <QObject>

class GlobalSimulationSettings : public QObject, public GlobalSettings
{
public:
    static GlobalSimulationSettings& get();
    static const Settings& getSettings();

    void afterSettingsChanged(const SettingID& settingID);

signals:
    void simulationResetRequired();
    void discTypeDistributionChanged();
};

#endif /* GLOBALSIMULATIONSETTINGS_HPP */
