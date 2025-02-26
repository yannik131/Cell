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
    GlobalSettings();

    static GlobalSettings& get();

    static const Settings& getSettings();

private:
    void setSimulationTimeStep(const sf::Time& simulationTimeStep);

    void setSimulationTimeScale(float simulationTimeScale);

    void setGuiFPS(int guiFPS);

    void setCollisionUpdateTime(const sf::Time& collisionUpdateTime);

    void setNumberOfDiscs(int numberOfDiscs);

    void setDiscTypeDistribution(const std::map<DiscType, int>& discTypeDistribution);

    void setFrictionCoefficient(float frictionCoefficient);

    template <typename T>
    void throwIfNotInRange(const T& value, const T& min, const T& max, const std::string& valueName);

    void throwIfLocked();

    void lock();

    void unlock();

private:
    Settings settings_;

    bool locked_ = false;

    friend class DistributionAndReactionsDialog;
    friend class Simulation;
    friend class SimulationSettingsWidget;
};

#include "GlobalSettings.inl"

#endif /* GLOBALSETTINGS_HPP */