#include "GlobalSettings.hpp"
#include "ExceptionWithLocation.hpp"

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

/**
 * @brief Given any one of the reaction maps (decomposition, collision, exchange, ...) remove any entries where the
 * given DiscType is contained in the educts
 */
template <typename T> void eraseIfInEducts(T& reactionTable, const DiscType& discType)
{
    using KeyType = typename T::key_type;

    for (auto iter = reactionTable.begin(); iter != reactionTable.end();)
    {
        const Reaction& reaction = iter->second.front();
        if (reaction.getEduct1().hasSameIdAs(discType) ||
            (reaction.hasEduct2() && reaction.getEduct2().hasSameIdAs(discType)))
        {
            iter = reactionTable.erase(iter);
            continue;
        }
        ++iter;
    }
}

/**
 * @brief Actual implementation of removeDanglingReactions
 */
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

/**
 * @brief Given a new DiscType distribution, find any reactions in the given vector where any of the products/educts is
 * equal to an updated DiscType using the getters of Reaction. Then, assign the new DiscType with the respective setter.
 * To find the changed DiscType, DiscType::getId() is used.
 */
void updateDiscTypesInReactions(std::vector<Reaction>& reactions, const DiscType::map<int>& newDiscTypeDistribution)
{
    for (auto& reaction : reactions)
    {
        // If we find a disc type that has the same id as an updated one in any of the educts/products, we'll update
        // the respective educt/product
        std::vector<std::pair<std::function<DiscType()>, std::function<void(const DiscType&)>>> gettersSetters{
            {std::bind(&Reaction::getEduct1, &reaction),
             std::bind(&Reaction::setEduct1, &reaction, std::placeholders::_1)},
            {std::bind(&Reaction::getEduct2, &reaction),
             std::bind(&Reaction::setEduct2, &reaction, std::placeholders::_1)},
            {std::bind(&Reaction::getProduct1, &reaction),
             std::bind(&Reaction::setProduct1, &reaction, std::placeholders::_1)},
            {std::bind(&Reaction::getProduct2, &reaction),
             std::bind(&Reaction::setProduct2, &reaction, std::placeholders::_1)}};

        if (!reaction.hasProduct2())
            gettersSetters.erase(gettersSetters.begin() + 3);

        if (!reaction.hasEduct2())
            gettersSetters.erase(gettersSetters.begin() + 1);

        for (auto& [getter, setter] : gettersSetters)
        {
            auto iter = newDiscTypeDistribution.find(getter());
            if (iter != newDiscTypeDistribution.end())
                setter(iter->first);
        }
    }
}

/**
 * @brief Educts are the keys in the reaction tables, so try to find changed DiscTypes in the keys and replace those
 * with the updated ones (keys are immutable and need to be replaced)
 */
template <typename T> void updateDiscTypesInEducts(T& reactionTable, const DiscType::map<int>& newDiscTypeDistribution)
{
    if (reactionTable.empty())
        return;

    using KeyType = typename T::key_type;

    for (const auto& [discType, _] : newDiscTypeDistribution)
    {
        // Find all keys that contain a possibly updated DiscType (for combination/exchange reactions, there might be
        // several; for decomposition reactions, there will only be 1)
        std::vector<KeyType> matches;
        for (const auto& [educts, reactions] : reactionTable)
        {
            const Reaction& reaction = reactions.front();
            if (reaction.getEduct1().hasSameIdAs(discType) ||
                (reaction.hasEduct2() && reaction.getEduct2().hasSameIdAs(discType)))
                matches.push_back(educts);
        }

        // Replace these keys with updates ones
        for (const auto& educts : matches)
        {
            auto iter = reactionTable.find(educts);
            auto reactions = iter->second;
            reactionTable.erase(iter);

            if constexpr (std::is_same_v<KeyType, std::pair<DiscType, DiscType>>)
            {
                const auto& newKey = std::make_pair(educts.first.hasSameIdAs(discType) ? discType : educts.first,
                                                    educts.second.hasSameIdAs(discType) ? discType : educts.second);
                reactionTable[newKey] = reactions;
            }
            else
                reactionTable[discType] = reactions;
        }
    }
}

template <typename T>
void updateDiscTypesInReactions(T& reactionTable, const DiscType::map<int>& newDiscTypeDistribution)
{
    // Step 1: Update disc types in all reactions
    for (auto& [educts, reactions] : reactionTable)
        updateDiscTypesInReactions(reactions, newDiscTypeDistribution);

    // Step 2: Update disc types in educts (keys)
    updateDiscTypesInEducts(reactionTable, newDiscTypeDistribution);
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

GlobalSettings::GlobalSettings()
{
    // TODO save settings as json, load default
    DiscType A("A", sf::Color::Green, 5, 5);
    DiscType B("B", sf::Color::Red, 10, 5);
    DiscType C("C", sf::Color::Blue, 12, 5);
    DiscType D("D", sf::Color::Magenta, 15, 5);

    settings_.discTypeDistribution_[A] = 100;
    settings_.discTypeDistribution_[B] = 0;
    settings_.discTypeDistribution_[C] = 0;
    settings_.discTypeDistribution_[D] = 0;

    addReaction(Reaction{A, std::nullopt, B, C, 1e-2f});
    addReaction(Reaction{B, C, D, std::nullopt, 1e-2f});
    addReaction(Reaction{D, std::nullopt, A, B, 1e-2f});
    addReaction(Reaction{B, A, C, std::nullopt, 1e-2f});
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

    switch (reaction.getType())
    {
    case Reaction::Type::Decomposition:
        addReactionToVector(settings_.decompositionReactions_[reaction.getEduct1()], reaction);
        break;
    case Reaction::Type::Combination:
        addReactionToVector(settings_.combinationReactions_[std::make_pair(reaction.getEduct1(), reaction.getEduct2())],
                            reaction);
        if (!reaction.getEduct1().hasSameIdAs(reaction.getEduct2()))
            addReactionToVector(
                settings_.combinationReactions_[std::make_pair(reaction.getEduct2(), reaction.getEduct1())], reaction);
        break;
    case Reaction::Type::Exchange:
        addReactionToVector(settings_.exchangeReactions_[std::make_pair(reaction.getEduct1(), reaction.getEduct2())],
                            reaction);
        if (!reaction.getEduct1().hasSameIdAs(reaction.getEduct2()))
            addReactionToVector(
                settings_.exchangeReactions_[std::make_pair(reaction.getEduct2(), reaction.getEduct1())], reaction);
        break;
    }

    useCallback(SettingID::Reactions);
}

void GlobalSettings::clearReactions()
{
    throwIfLocked();

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

    ::removeDanglingReactions(settings_.decompositionReactions_, removedDiscTypes);
    ::removeDanglingReactions(settings_.combinationReactions_, removedDiscTypes);
    ::removeDanglingReactions(settings_.exchangeReactions_, removedDiscTypes);
}

void GlobalSettings::updateDiscTypesInReactions(const DiscType::map<int>& newDiscTypeDistribution)
{
    ::updateDiscTypesInReactions(settings_.decompositionReactions_, newDiscTypeDistribution);
    ::updateDiscTypesInReactions(settings_.combinationReactions_, newDiscTypeDistribution);
    ::updateDiscTypesInReactions(settings_.exchangeReactions_, newDiscTypeDistribution);
}

void GlobalSettings::useCallback(const SettingID& settingID)
{
    if (callback_)
        callback_(settingID);
}
