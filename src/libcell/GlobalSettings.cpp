#include "GlobalSettings.hpp"

#include <algorithm>
#include <type_traits>
#include <vector>

void GlobalSettings::setCallback(const std::function<void(const SettingID& settingID)>& functor)
{
    GlobalSettings::get().callback_ = functor;
}

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

    addReaction(Reaction{A, B, C, std::nullopt, 1e-3f});
    addReaction(Reaction{A, B, D, std::nullopt, 2e-3f});

    addReaction(Reaction{C, std::nullopt, A, B, 1e-3f});
    addReaction(Reaction{D, std::nullopt, A, B, 5e-3f});
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

    useCallback(SettingID::SimulationTimeStep);
}

void GlobalSettings::setSimulationTimeScale(float simulationTimeScale)
{
    throwIfLocked();
    throwIfNotInRange(simulationTimeScale, SettingsLimits::MinSimulationTimeScale,
                      SettingsLimits::MaxSimulationTimeScale, "simulation time scale");

    settings_.simulationTimeScale_ = simulationTimeScale;

    useCallback(SettingID::SimulationTimeScale);
}

void GlobalSettings::setNumberOfDiscs(int numberOfDiscs)
{
    throwIfLocked();
    throwIfNotInRange(numberOfDiscs, SettingsLimits::MinNumberOfDiscs, SettingsLimits::MaxNumberOfDiscs,
                      "number of discs");

    settings_.numberOfDiscs_ = numberOfDiscs;

    useCallback(SettingID::NumberOfDiscs);
}

void GlobalSettings::setDiscTypeDistribution(const std::map<DiscType, int>& discTypeDistribution)
{
    throwIfLocked();

    if (discTypeDistribution.empty())
        throw std::runtime_error("Disc type distribution cannot be empty");

    int totalPercent = 0;
    for (const auto& [type, percent] : discTypeDistribution)
        totalPercent += percent;

    if (totalPercent != 100)
        throw std::runtime_error("Percentages for disc type distribution don't add up to 100. They add up to " +
                                 std::to_string(totalPercent));

    removeDanglingReactions(discTypeDistribution);

    settings_.discTypeDistribution_ = discTypeDistribution;

    useCallback(SettingID::DiscTypeDistribution);
}

void GlobalSettings::addReaction(const Reaction& reaction)
{
    throwIfLocked();

    switch (reaction.getType())
    {
    case Reaction::Type::Decomposition:
        addReactionToVector(settings_.decompositionReactions_[reaction.getEduct1()], reaction);
        break;
    case Reaction::Type::Combination:
        addReactionToVector(settings_.combinationReactions_[std::make_pair(reaction.getEduct1(), reaction.getEduct2())],
                            reaction);
        addReactionToVector(settings_.combinationReactions_[std::make_pair(reaction.getEduct2(), reaction.getEduct1())],
                            reaction);
        break;
    case Reaction::Type::Exchange:
        addReactionToVector(settings_.exchangeReactions_[std::make_pair(reaction.getEduct1(), reaction.getEduct2())],
                            reaction);
        addReactionToVector(settings_.exchangeReactions_[std::make_pair(reaction.getEduct2(), reaction.getEduct1())],
                            reaction);
        break;
    }

    useCallback(SettingID::Reactions);
}

void GlobalSettings::clearReactions()
{
    settings_.decompositionReactions_.clear();
    settings_.combinationReactions_.clear();
    settings_.exchangeReactions_.clear();

    useCallback(SettingID::Reactions);
}

void GlobalSettings::setFrictionCoefficient(float frictionCoefficient)
{
    throwIfLocked();
    throwIfNotInRange(frictionCoefficient, SettingsLimits::MinFrictionCoefficient,
                      SettingsLimits::MaxFrictionCoefficient, "friction coefficient");

    settings_.frictionCoefficient = frictionCoefficient;

    useCallback(SettingID::FrictionCoefficient);
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

void GlobalSettings::useCallback(const SettingID& settingID)
{
    if (callback_)
        callback_(settingID);
}
