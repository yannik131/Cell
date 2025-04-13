#ifndef GLOBALSETTINGS_HPP
#define GLOBALSETTINGS_HPP

#include "DiscType.hpp"
#include "Settings.hpp"

#include <SFML/System/Time.hpp>

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
    static DiscType getDiscTypeByName(const std::string& name);

    virtual ~GlobalSettings() = default;

protected:
    GlobalSettings();
    virtual void afterSettingsChanged(const SettingID& settingID);

protected:
    Settings settings_;

private:
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

private:
    bool locked_ = false;

    friend class DiscTypeDistributionTableModel;
    friend class ReactionsTableModel;
    friend class ReactionsDialog;
    friend class Simulation;
    friend class SimulationSettingsWidget;
    friend void setBenchmarkSettings();
};

template <typename T> void throwIfNotInRange(const T& value, const T& min, const T& max, const std::string& valueName);

#include "GlobalSettings.inl"

#endif /* GLOBALSETTINGS_HPP */