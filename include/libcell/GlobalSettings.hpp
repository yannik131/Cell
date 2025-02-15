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

private:
    void setSimulationTimeStep(const sf::Time& simulationTimeStep);

    void setSimulationTimeScale(float simulationTimeScale);

    void setGuiFPS(int guiFPS);

    void setCollisionUpdateTime(const sf::Time& collisionUpdateTime);

    void setNumberOfDiscs(int numberOfDiscs);

    void setDiscTypeDistribution(const std::map<DiscType, int>& discTypeDistribution);

    template <typename T>
    void throwIfNotInRange(const T& value, const T& min, const T& max, const std::string& valueName);

    void throwIfLocked();

    void lock();

    void unlock();

private:
    Settings settings_;

    bool locked_ = false;
};

#include "GlobalSettings.inl"

#endif /* GLOBALSETTINGS_HPP */