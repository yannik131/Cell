#ifndef GLOBALSETTINGS_HPP
#define GLOBALSETTINGS_HPP

#include "DiscType.hpp"
#include "Settings.hpp"

#include <SFML/System/Time.hpp>

#include <functional>
#include <map>
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

class GlobalSettings
{
public:
    static GlobalSettings& get();

    static const Settings& getSettings();

    static void setCallback(const std::function<void(const SettingID& settingID)>& functor);

private:
    GlobalSettings();

    void setSimulationTimeStep(const sf::Time& simulationTimeStep);

    void setSimulationTimeScale(float simulationTimeScale);

    void setNumberOfDiscs(int numberOfDiscs);

    void setDiscTypeDistribution(const std::map<DiscType, int>& discTypeDistribution);

    void addReaction(const Reaction& reaction);

    void clearReactions();

    void setFrictionCoefficient(float frictionCoefficient);

    void throwIfLocked();

    void lock();

    void unlock();

    void removeDanglingReactions(const std::map<DiscType, int>& newDiscTypeDistribution);

    void useCallback(const SettingID& settingID);

private:
    Settings settings_;

    bool locked_ = false;
    std::function<void(const SettingID& settingID)> callback_;

    friend class DiscTypeDistributionTableModel;
    friend class ReactionsTableModel;
    friend class Simulation;
    friend class SimulationControlWidget;
    friend void setBenchmarkSettings();
};

template <typename T> void throwIfNotInRange(const T& value, const T& min, const T& max, const std::string& valueName);

#include "GlobalSettings.inl"

#endif /* GLOBALSETTINGS_HPP */