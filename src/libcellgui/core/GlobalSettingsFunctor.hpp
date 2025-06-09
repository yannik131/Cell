#ifndef GLOBALSETTINGSFUNCTOR_HPP
#define GLOBALSETTINGSFUNCTOR_HPP

#include "GlobalSettings.hpp"

#include <QObject>

// TODO documentation once generic settings class is here

/**
 * @brief Callback to be inserted into the libcell::GlobalSettings to interact with changed settings via Qt's
 * signals-slots mechanism
 */
class GlobalSettingsFunctor : public QObject
{
    Q_OBJECT
public:
    static GlobalSettingsFunctor& get();

    /**
     * @brief Called by libcell::GlobalSettings with the appropriate ID if a setting was changed
     */
    void operator()(const SettingID& settingID);

signals:
    void numberOfDiscsChanged();
    void discTypeDistributionChanged();

private:
    GlobalSettingsFunctor() = default;
};

#endif /* GLOBALSETTINGSFUNCTOR_HPP */
