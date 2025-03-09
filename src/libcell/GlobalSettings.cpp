#include "GlobalSettings.hpp"

#include <algorithm>
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
        if (type.name_.empty())
            throw std::runtime_error("Disc type name can't be empty");
    }

    if (totalPercent != 100)
        throw std::runtime_error("Percentages for disc type distribution don't add up to 100. They adds up to " +
                                 std::to_string(totalPercent));

    removeDanglingReactions(discTypeDistribution);

    settings_.discTypeDistribution_ = discTypeDistribution;
}

void GlobalSettings::addReaction(const Reaction& reaction)
{
    throwIfLocked();
    throwIfNotInRange(reaction.probability_, 0.f, 1.f, "reaction probability");

    if (isValid(reaction.educt1_) && !isValid(reaction.educt2_) && isValid(reaction.product1_) &&
        isValid(reaction.product2_))
    {
        addReactionToVector(settings_.decompositionReactions_[reaction.educt1_], reaction);
    }
    else if (isValid(reaction.educt1_) && isValid(reaction.educt2_) && isValid(reaction.product1_) &&
             !isValid(reaction.product2_))
    {
        addReactionToVector(settings_.combinationReactions_[std::make_pair(reaction.educt1_, reaction.educt2_)],
                            reaction);
        addReactionToVector(settings_.combinationReactions_[std::make_pair(reaction.educt2_, reaction.educt1_)],
                            reaction);
    }
    else if (isValid(reaction.educt1_) && isValid(reaction.educt2_) && isValid(reaction.product1_) &&
             isValid(reaction.product2_))
    {
        addReactionToVector(settings_.exchangeReactions_[std::make_pair(reaction.educt1_, reaction.educt2_)], reaction);
        addReactionToVector(settings_.exchangeReactions_[std::make_pair(reaction.educt2_, reaction.educt1_)], reaction);
    }
    else
    {
        throw std::runtime_error("Invalid reaction: Neither of type A -> B + C, A + B -> C or A + B -> C + D");
    }
}

void GlobalSettings::clearReactions()
{
    settings_.decompositionReactions_.clear();
    settings_.combinationReactions_.clear();
    settings_.exchangeReactions_.clear();
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

template <typename T> void eraseIfInEducts(T& reactionTable, const DiscType& discType)
{
    for (auto iter = reactionTable.begin(); iter != reactionTable.end();)
    {
        if (iter->first.first == discType || iter->first.second == discType)
            iter = reactionTable.erase(iter);
        else
            ++iter;
    }
}

void GlobalSettings::removeDanglingReactions(const std::map<DiscType, int>& newDiscTypeDistribution)
{
    std::vector<DiscType> removedDiscTypes;
    std::vector<DiscType> remainingDiscTypes;

    for (const auto& [type, percent] : settings_.discTypeDistribution_)
    {
        if (newDiscTypeDistribution.find(type) == newDiscTypeDistribution.end())
            removedDiscTypes.push_back(type);
        else
            remainingDiscTypes.push_back(type);
    }

    for (const auto& removedDiscType : removedDiscTypes)
    {
        // Step 1: Erase all reactions that have the removed disc type as an educt
        eraseIfInEducts(settings_.decompositionReactions_, removedDiscType);
        eraseIfInEducts(settings_.combinationReactions_, removedDiscType);
        eraseIfInEducts(settings_.exchangeReactions_, removedDiscType);

        // Step 2: Erase all reactions that have the removed disc type as a product
        for (auto& [educt, products] : settings_.decompositionReactions_)
            removeReactionsFromVector(products, removedDiscType);

        for (auto& [educts, products] : settings_.combinationReactions_)
            removeReactionsFromVector(products, removedDiscType);

        for (auto& [educts, products] : settings_.exchangeReactions_)
            removeReactionsFromVector(products, removedDiscType);
    }
}
