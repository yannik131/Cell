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

DiscType GlobalSettings::getDiscTypeByName(const std::string& name)
{
    for (const auto& [discType, frequency] : getSettings().discTypeDistribution_)
    {
        if (discType.name_ == name)
            return discType;
    }

    throw std::runtime_error("No disc type found for name \"" + name + "\"");
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
    // TODO Remove all reactions that have disc types that are not contained in the new distribution
    throwIfLocked();

    if (discTypeDistribution.empty())
        throw std::runtime_error("Disc type distribution cannot be empty");

    int totalPercent = 0;
    for (const auto& [type, percent] : discTypeDistribution)
    {
        if (percent < 0)
            throw std::runtime_error("Percentage for disc type\"" + type.name_ + "\" is smaller than 0");

        totalPercent += percent;

        throwIfNotInRange(type.mass_, DiscTypeLimits::MinMass, DiscTypeLimits::MaxMass,
                          "mass for disc type \"" + type.name_ + "\"");
        throwIfNotInRange(type.radius_, DiscTypeLimits::MinRadius, DiscTypeLimits::MaxRadius,
                          "radius for disc type \"" + type.name_ + "\"");
    }

    if (totalPercent != 100)
        throw std::runtime_error("Percentages for disc type distribution don't add up to 100. They adds up to " +
                                 std::to_string(totalPercent));

    settings_.discTypeDistribution_ = discTypeDistribution;
}

void GlobalSettings::setCombinationReactions(
    const std::map<std::pair<DiscType, DiscType>, std::vector<std::pair<DiscType, float>>>& combinationReactions)
{
    throwIfLocked();

    for (const auto& [educts, products] : combinationReactions)
    {
        for (const auto& [product, probability] : products)
            throwIfNotInRange(probability, 0.f, 1.f, "probability");
    }

    settings_.combinationReactionTable_ = combinationReactions;
}

void GlobalSettings::setDecompositionReactions(
    const std::map<DiscType, std::vector<std::pair<std::pair<DiscType, DiscType>, float>>>& decompositionReactions)
{
    throwIfLocked();

    for (const auto& [educt, products] : decompositionReactions)
    {
        for (const auto& [product, probability] : products)
            throwIfNotInRange(probability, 0.f, 1.f, "probability");
    }

    settings_.decompositionReactionTable_ = decompositionReactions;
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
