#ifndef GLOBALSETTINGSFUNCTOR_HPP
#define GLOBALSETTINGSFUNCTOR_HPP

#include "GlobalSettings.hpp"

#include <QObject>

class GlobalSettingsFunctor : public QObject
{
    Q_OBJECT
public:
    static GlobalSettingsFunctor& get();

    void operator()(const SettingID& settingID);

signals:
    void simulationResetRequired();
    void discTypeDistributionChanged();

private:
    GlobalSettingsFunctor() = default;
};

#endif /* GLOBALSETTINGSFUNCTOR_HPP */
