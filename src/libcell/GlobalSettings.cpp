#include "GlobalSettings.hpp"

#include <vector>

GlobalSettings::GlobalSettings()
{
}

GlobalSettings& GlobalSettings::get()
{
    static GlobalSettings instance;

    return instance;
}

const Settings& GlobalSettings::getSettings()
{
    return get().settings_;
}

void GlobalSettings::setSimulationTimeStep(const sf::Time& simulationTimeStep)
{
    throwIfLocked();
    throwIfNotInRange(simulationTimeStep, SettingsLimits::MinSimulationTimeStep, SettingsLimits::MaxSimulationTimeStep,
                      "simulation time step");

    settings_.simulationTimeStep_ = simulationTimeStep;
}

void GlobalSettings::setSimulationTimeScale(float simulationTimeScale)
{
    throwIfLocked();
    throwIfNotInRange(simulationTimeScale, SettingsLimits::MinSimulationTimeScale,
                      SettingsLimits::MaxSimulationTimeScale, "simulation time scale");

    settings_.simulationTimeScale_ = simulationTimeScale;
}

void GlobalSettings::setGuiFPS(int guiFPS)
{
    throwIfLocked();
    throwIfNotInRange(guiFPS, SettingsLimits::MinGuiFPS, SettingsLimits::MaxGuiFPS, "GUI FPS");

    settings_.guiFPS_ = guiFPS;
}

void GlobalSettings::setCollisionUpdateTime(const sf::Time& collisionUpdateTime)
{
    throwIfLocked();
    throwIfNotInRange(collisionUpdateTime, SettingsLimits::MinCollisionUpdateTime,
                      SettingsLimits::MaxCollisionUpdateTime, "collision update time");

    settings_.collisionUpdateTime_ = collisionUpdateTime;
}

void GlobalSettings::setNumberOfDiscs(int numberOfDiscs)
{
    throwIfLocked();
    throwIfNotInRange(numberOfDiscs, SettingsLimits::MinNumberOfDiscs, SettingsLimits::MaxNumberOfDiscs,
                      "number of discs");

    settings_.numberOfDiscs_ = numberOfDiscs;
}

void GlobalSettings::setDiscTypeDistribution(const std::map<DiscType, int>& discTypeDistribution)
{
    throwIfLocked();

    if (discTypeDistribution.empty())
        throw std::runtime_error("Disc type distribution cannot be empty");

    int totalPercent = 0;
    for (const auto& [type, percent] : discTypeDistribution)
    {
        if (percent <= 0)
            throw std::runtime_error("Percentage for disc type\"" + type.name_ + "\" is equal to or smaller than 0");

        totalPercent += percent;

        throwIfNotInRange(type.mass_, DiscTypeLimits::MinMass, DiscTypeLimits::MaxMass,
                          "mass for disc type \"" + type.name_ + "\"");
        throwIfNotInRange(type.radius_, DiscTypeLimits::MinRadius, DiscTypeLimits::MaxRadius,
                          "radius for disc type \"" + type.name_ + "\"");
    }

    if (totalPercent != 100)
        throw std::runtime_error("Percentages for disc type distribution doesn't add up to 100");

    settings_.discTypeDistribution_ = discTypeDistribution;
}

void GlobalSettings::setFrictionCoefficient(float frictionCoefficient)
{
    throwIfLocked();
    throwIfNotInRange(frictionCoefficient, SettingsLimits::MinFrictionCoefficient,
                      SettingsLimits::MaxFrictionCoefficient, "friction coefficient");

    settings_.frictionCoefficient = frictionCoefficient;
}

void GlobalSettings::throwIfLocked()
{
    if (locked_)
        throw std::runtime_error("Settings are locked");
}

void GlobalSettings::lock()
{
    locked_ = true;
}

void GlobalSettings::unlock()
{
    locked_ = false;
}
