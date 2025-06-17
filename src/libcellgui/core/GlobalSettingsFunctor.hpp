#ifndef GLOBALSETTINGSFUNCTOR_HPP
#define GLOBALSETTINGSFUNCTOR_HPP

#include "GlobalSettings.hpp"

#include <QObject>

// TODO documentation once generic settings class is here

/**
 * @brief Callback to be inserted into the cell::GlobalSettings to interact with changed settings via Qt's
 * signals-slots mechanism
 */
class GlobalSettingsFunctor : public QObject
{
    Q_OBJECT
public:
    static GlobalSettingsFunctor& get();

    /**
     * @brief Called by cell::GlobalSettings with the appropriate ID if a setting was changed
     */
    void operator()(const cell::SettingID& settingID);

signals:
    void numberOfDiscsChanged();
    void discTypeDistributionChanged();
    void simulationTimeScaleChanged();
    void simulationTimeStepChanged();

private:
    GlobalSettingsFunctor() = default;
};

#endif /* GLOBALSETTINGSFUNCTOR_HPP */
