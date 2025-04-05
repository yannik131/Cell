#include "GlobalSettings.hpp"

#include <algorithm>
#include <type_traits>
#include <vector>

GlobalSettings::GlobalSettings()
{

    // TODO save settings as json, load default
    DiscType A("A", sf::Color::Green, 5, 5);
    DiscType B("B", sf::Color::Red, 10, 5);
    DiscType C("C", sf::Color::Blue, 12, 5);
    DiscType D("D", sf::Color::Yellow, 15, 5);

    settings_.discTypeDistribution_[A] = 50;
    settings_.discTypeDistribution_[B] = 30;
    settings_.discTypeDistribution_[C] = 10;
    settings_.discTypeDistribution_[D] = 10;

    addReaction({.educt1_ = A, .educt2_ = B, .product1_ = C, .probability_ = 0.001f});
    addReaction({.educt1_ = A, .educt2_ = B, .product1_ = D, .probability_ = 0.002f});

    addReaction({.educt1_ = C, .product1_ = A, .product2_ = B, .probability_ = 0.001f});
    addReaction({.educt1_ = D, .product1_ = A, .product2_ = B, .probability_ = 0.005f});
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

void GlobalSettings::setPlotTimeInterval(const sf::Time& plotTimeInterval)
{
    throwIfLocked();
    throwIfNotInRange(plotTimeInterval, SettingsLimits::MinPlotTimeInterval, SettingsLimits::MaxPlotTimeInterval,
                      "collision update time");

    settings_.plotTimeInterval_ = plotTimeInterval;
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
        throw std::runtime_error("Percentages for disc type distribution don't add up to 100. They add up to " +
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
    using KeyType = typename T::key_type;

    for (auto iter = reactionTable.begin(); iter != reactionTable.end();)
    {
        bool eraseKey;

        if constexpr (std::is_same_v<KeyType, std::pair<DiscType, DiscType>>)
            eraseKey = iter->first.first == discType || iter->first.second == discType;
        else
            eraseKey = iter->first == discType;

        if (eraseKey)
        {
            iter = reactionTable.erase(iter);
            continue;
        }
        ++iter;
    }
}

template <typename T> void removeDanglingReactions(T& reactionTable, const std::vector<DiscType>& removedDiscTypes)
{
    for (const auto& removedDiscType : removedDiscTypes)
    {
        // Step 1: Erase all reactions that have the removed disc type as an educt
        eraseIfInEducts(reactionTable, removedDiscType);

        // Step 2: Erase all reactions that have the removed disc type as a product
        for (auto iter = reactionTable.begin(); iter != reactionTable.end();)
        {
            removeReactionsFromVector(iter->second, removedDiscType);
            if (iter->second.empty())
                iter = reactionTable.erase(iter);
            else
                ++iter;
        }
    }
}

void GlobalSettings::removeDanglingReactions(const std::map<DiscType, int>& newDiscTypeDistribution)
{
    std::vector<DiscType> removedDiscTypes;

    for (const auto& [type, percent] : settings_.discTypeDistribution_)
    {
        if (newDiscTypeDistribution.find(type) == newDiscTypeDistribution.end())
            removedDiscTypes.push_back(type);
    }

    ::removeDanglingReactions(settings_.decompositionReactions_, removedDiscTypes);
    ::removeDanglingReactions(settings_.combinationReactions_, removedDiscTypes);
    ::removeDanglingReactions(settings_.exchangeReactions_, removedDiscTypes);
}
