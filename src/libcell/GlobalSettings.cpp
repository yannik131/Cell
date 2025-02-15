#include "GlobalSettings.hpp"

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
    throwIfNotInRange(simulationTimeStep, settings_.MinSimulationTimeStep, settings_.MaxSimulationTimeStep,
                      "simulation time step");

    settings_.simulationTimeStep_ = simulationTimeStep;
}

void GlobalSettings::setSimulationTimeScale(float simulationTimeScale)
{
    throwIfLocked();
    throwIfNotInRange(simulationTimeScale, settings_.MinSimulationTimeScale, settings_.MaxSimulationTimeScale,
                      "simulation time scale");

    settings_.simulationTimeScale_ = simulationTimeScale;
}

void GlobalSettings::setGuiFPS(int guiFPS)
{
    throwIfLocked();
    throwIfNotInRange(guiFPS, settings_.MinGuiFPS, settings_.MaxGuiFPS, "GUI FPS");

    settings_.guiFPS_ = guiFPS;
}

void GlobalSettings::setCollisionUpdateTime(const sf::Time& collisionUpdateTime)
{
    throwIfLocked();
    throwIfNotInRange(collisionUpdateTime, settings_.MinCollisionUpdateTime, settings_.MaxCollisionUpdateTime,
                      "collision update time");

    settings_.collisionUpdateTime_ = collisionUpdateTime;
}

void GlobalSettings::setNumberOfDiscs(int numberOfDiscs)
{
    throwIfLocked();
    throwIfNotInRange(numberOfDiscs, settings_.MinNumberOfDiscs, settings_.MaxNumberOfDiscs, "number of discs");

    settings_.numberOfDiscs_ = numberOfDiscs;
}

void GlobalSettings::setDiscTypeDistribution(const std::map<DiscType, int>& discTypeDistribution)
{
    throwIfLocked();

    if (discTypeDistribution.empty())
        throw std::runtime_error("Disc type distribution can not be empty");

    int totalPercent = 0;
    for (const auto& [type, percent] : settings_.discTypeDistribution_)
    {
        if (percent <= 0)
            throw std::runtime_error("Percentage for disc type\"" + type.name_ + "\" is equal to or smaller than 0");

        totalPercent += percent;

        throwIfNotInRange(type.mass_, settings_.discTypeLimits.MinMass, settings_.discTypeLimits.MaxMass,
                          "mass for disc type \"" + type.name_ + "\"");
        throwIfNotInRange(type.radius_, settings_.discTypeLimits.MinRadius, settings_.discTypeLimits.MaxRadius,
                          "radius for disc type \"" + type.name_ + "\"");
    }

    if (totalPercent != 100)
        throw std::runtime_error("Percentages for disc type distribution doesn't add up to 100");

    settings_.discTypeDistribution_ = discTypeDistribution;
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
