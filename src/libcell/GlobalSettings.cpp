#include "GlobalSettings.hpp"
#include "ExceptionWithLocation.hpp"

#include "ReactionTable.hpp"
#include <algorithm>
#include <functional>
#include <set>
#include <type_traits>
#include <vector>

namespace
{

/**
 * @returns Returns an optional containing the first duplicate by name found, if there was any
 */
std::optional<std::string> getFirstDuplicateByName(const DiscType::map<int>& discTypeDistribution)
{
    std::set<std::string> uniqueNames;
    for (const auto& [discType, frequency] : discTypeDistribution)
    {
        if (uniqueNames.contains(discType.getName()))
            return discType.getName();

        uniqueNames.insert(discType.getName());
    }

    return std::nullopt;
}

/**
 * @returns Sum of all values of the given map
 */
int calculateFrequencySum(const DiscType::map<int>& discTypeDistribution)
{
    int totalPercent = 0;
    for (const auto& [type, percent] : discTypeDistribution)
        totalPercent += percent;

    return totalPercent;
}

void throwIfNotInDistribution(const DiscType& discType, const DiscType::map<int>& distribution)
{
    for (const auto& [other, frequency] : distribution)
    {
        if (other == discType)
            return;
    }

    throw ExceptionWithLocation("DiscType \"" + discType.getName() + "\" not found in distribution");
}

} // namespace

void GlobalSettings::setCallback(const std::function<void(const SettingID& settingID)>& functor)
{
    GlobalSettings::get().callback_ = functor;
}

void GlobalSettings::restoreDefault()
{
    settings_ = Settings();

    // TODO save settings as json, load default
    DiscType A("A", sf::Color::Green, 5, 10);
    DiscType B("B", sf::Color::Red, 10, 5);
    DiscType C("C", sf::Color::Blue, 12, 5);
    DiscType D("D", sf::Color::Magenta, 15, 10);

    settings_.discTypeDistribution_[A] = 100;
    settings_.discTypeDistribution_[B] = 0;
    settings_.discTypeDistribution_[C] = 0;
    settings_.discTypeDistribution_[D] = 0;

    addReaction(Reaction{A, std::nullopt, B, C, 1e-2f});
    addReaction(Reaction{B, C, D, std::nullopt, 1e-2f});
    addReaction(Reaction{B, A, C, D, 1e-2f});
}

GlobalSettings::GlobalSettings()
{
    restoreDefault();
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

void GlobalSettings::setDiscTypeDistribution(const DiscType::map<int>& discTypeDistribution)
{
    throwIfLocked();

    if (discTypeDistribution.empty())
        throw ExceptionWithLocation("Disc type distribution cannot be empty");

    if (const auto& duplicateName = getFirstDuplicateByName(discTypeDistribution))
        throw ExceptionWithLocation("Duplicate disc type found: " + *duplicateName);

    if (int totalPercent = calculateFrequencySum(discTypeDistribution); totalPercent != 100)
        throw ExceptionWithLocation("Percentages for disc type distribution don't add up to 100. They add up to " +
                                    std::to_string(totalPercent));

    removeDanglingReactions(discTypeDistribution);
    updateDiscTypesInReactions(discTypeDistribution);

    settings_.discTypeDistribution_ = discTypeDistribution;

    useCallback(SettingID::DiscTypeDistribution);
}

void GlobalSettings::addReaction(const Reaction& reaction)
{
    throwIfLocked();

    throwIfNotInDistribution(reaction.getEduct1(), settings_.discTypeDistribution_);
    throwIfNotInDistribution(reaction.getProduct1(), settings_.discTypeDistribution_);

    if (reaction.hasEduct2())
        throwIfNotInDistribution(reaction.getEduct2(), settings_.discTypeDistribution_);

    if (reaction.hasProduct2())
        throwIfNotInDistribution(reaction.getProduct2(), settings_.discTypeDistribution_);

    reaction.validate();

    settings_.reactionTable_.addReaction(reaction);

    useCallback(SettingID::Reactions);
}

void GlobalSettings::clearReactions()
{
    throwIfLocked();

    settings_.reactionTable_.clear();

    useCallback(SettingID::Reactions);
}

void GlobalSettings::throwIfLocked()
{
    if (locked_)
        throw ExceptionWithLocation("Settings are locked");
}

void GlobalSettings::lock()
{
    locked_ = true;
}

void GlobalSettings::unlock()
{
    locked_ = false;
}

bool GlobalSettings::isLocked() const
{
    return locked_;
}

const std::vector<Reaction>& GlobalSettings::getReactions() const
{
    return settings_.reactionTable_.getReactions();
}

/**
 * @brief Given a reaction map, remove all entries where a removed DiscType is either in the educts or products of any
 * of the reactions
 */
void GlobalSettings::removeDanglingReactions(const DiscType::map<int>& newDiscTypeDistribution)
{
    std::vector<DiscType> removedDiscTypes;

    for (const auto& [type, percent] : settings_.discTypeDistribution_)
    {
        if (newDiscTypeDistribution.find(type) == newDiscTypeDistribution.end())
            removedDiscTypes.push_back(type);
    }

    if (!removedDiscTypes.empty())
        settings_.reactionTable_.removeDiscTypes(removedDiscTypes);
}

void GlobalSettings::updateDiscTypesInReactions(const DiscType::map<int>& newDiscTypeDistribution)
{
    DiscType::map<DiscType> updatedDiscTypes;
    for (const auto& [oldDiscType, frequency] : settings_.discTypeDistribution_)
    {
        auto iter = newDiscTypeDistribution.find(oldDiscType);
        if (iter != newDiscTypeDistribution.end() && !iter->first.equalsTo(oldDiscType))
            updatedDiscTypes.emplace(oldDiscType, iter->first);
    }

    settings_.reactionTable_.updateDiscTypes(updatedDiscTypes);
}

void GlobalSettings::useCallback(const SettingID& settingID)
{
    if (callback_)
        callback_(settingID);
}
