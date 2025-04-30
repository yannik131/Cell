#ifndef GLOBALSETTINGS_HPP
#define GLOBALSETTINGS_HPP

#include "DiscType.hpp"
#include "ExceptionMacro.hpp"
#include "Settings.hpp"
#include "StringUtils.hpp"

#include <SFML/System/Time.hpp>

#include <functional>
#include <map>
#include <stdexcept>
#include <string>

enum SettingID
{
    SimulationTimeStep = 1 << 0,
    SimulationTimeScale = 1 << 1,
    NumberOfDiscs = 1 << 2,
    DiscTypeDistribution = 1 << 3,
    Reactions = 1 << 4,
    FrictionCoefficient = 1 << 5,
};

CUSTOM_EXCEPTION(ChangeDuringLockException)

class GlobalSettings
{
public:
    static GlobalSettings& get();

    static const Settings& getSettings();

    static void setCallback(const std::function<void(const SettingID& settingID)>& functor);

    void setSimulationTimeStep(const sf::Time& simulationTimeStep);

    void setSimulationTimeScale(float simulationTimeScale);

    void setNumberOfDiscs(int numberOfDiscs);

    void setFrictionCoefficient(float frictionCoefficient);

    void setDiscTypeDistribution(const std::map<DiscType, int>& discTypeDistribution);

    void addReaction(const Reaction& reaction);

    void clearReactions();

    void throwIfLocked();

    void lock();

    void unlock();

private:
    GlobalSettings();

    void removeDanglingReactions(const std::map<DiscType, int>& newDiscTypeDistribution);

    void updateDiscTypesInReactions(const std::map<DiscType, int>& newDiscTypeDistribution);

    void useCallback(const SettingID& settingID);

private:
    Settings settings_;

    bool locked_ = false;
    std::function<void(const SettingID& settingID)> callback_;
};

template <typename T> void throwIfNotInRange(const T& value, const T& min, const T& max, const std::string& valueName)
{
    using StringUtils::toString;

    if (value < min || value > max)
        throw std::runtime_error("Value for \"" + valueName + "\" out of range: Must be between \"" + toString(min) +
                                 "\" and \"" + toString(max) + "\", but is \"" + toString(value) + "\"");
}

#endif /* GLOBALSETTINGS_HPP */
