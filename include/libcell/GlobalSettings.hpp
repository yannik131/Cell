#ifndef GLOBALSETTINGS_HPP
#define GLOBALSETTINGS_HPP

#include "DiscType.hpp"
#include "Settings.hpp"

#include <SFML/System/Time.hpp>

#include <map>
#include <string>

class GlobalSettings
{
public:
    static GlobalSettings& get();

    static const Settings& getSettings();

    static DiscType getDiscTypeByName(const std::string& name);

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

private:
    Settings settings_;

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