#ifndef GLOBALSETTINGSFUNCTOR_HPP
#define GLOBALSETTINGSFUNCTOR_HPP

#include "GlobalSettings.hpp"

#include <QObject>

// TODO documentation once generic settings class is here

class GlobalSettingsFunctor : public QObject
{
    Q_OBJECT
public:
    static GlobalSettingsFunctor& get();

    void operator()(const SettingID& settingID);

signals:
    void numberOfDiscsChanged();
    void discTypeDistributionChanged();

private:
    GlobalSettingsFunctor() = default;
};

#endif /* GLOBALSETTINGSFUNCTOR_HPP */
