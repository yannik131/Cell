#include "GlobalSettings.hpp"
#include "ExceptionWithLocation.hpp"
#include "ReactionTable.hpp"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <fstream>
#include <functional>
#include <set>
#include <type_traits>
#include <vector>

using json = nlohmann::json;

namespace cell
{

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
        if (uniqueNames.contains(discType->getName()))
            return discType->getName();

        uniqueNames.insert(discType->getName());
    }

    return std::nullopt;
}

std::optional<std::string> getFirstInvalidByName(const DiscType::map<int>& discTypeDistribution,
                                                 const std::vector<DiscType>& discTypes)
{
    for (const auto& [discType, frequency] : discTypeDistribution)
    {
        if (std::find(discTypes.begin(), discTypes.end(), *discType) != discTypes.end())
            return discType->getName();
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

void throwIfNotInDiscTypes(const DiscType& discType, const std::vector<DiscType>& discTypes)
{
    if (std::find(discTypes.begin(), discTypes.end(), discType) != discTypes.end())
        throw ExceptionWithLocation("DiscType \"" + discType.getName() + "\" not found in distribution");
}

} // namespace

void GlobalSettings::setCallback(const std::function<void(const SettingID& settingID)>& functor)
{
    GlobalSettings::get().callback_ = functor;
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

void GlobalSettings::setSimulationTimeScale(double simulationTimeScale)
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

void GlobalSettings::setCellSize(int width, int height)
{
    throwIfLocked();
    throwIfNotInRange(width, SettingsLimits::MinCellWidth, SettingsLimits::MaxCellWidth, "cell width");
    throwIfNotInRange(height, SettingsLimits::MinCellHeight, SettingsLimits::MaxCellHeight, "cell height");

    settings_.cellWidth_ = width;
    settings_.cellHeight_ = height;

    useCallback(SettingID::CellSize);
}

void GlobalSettings::removeDiscType(const DiscType* discType)
{
    settings_.reactionTable_.removeDiscType(discType);
}

void GlobalSettings::setDiscTypeDistribution(const DiscType::map<int>& discTypeDistribution)
{
    throwIfLocked();

    if (discTypeDistribution.empty())
        throw ExceptionWithLocation("Disc type distribution cannot be empty");

    if (const auto& duplicateName = getFirstDuplicateByName(discTypeDistribution))
        throw ExceptionWithLocation("Duplicate disc type found: " + *duplicateName);

    if (const auto& invalidDiscTypeName = getFirstInvalidByName(discTypeDistribution, settings_.discTypes_))
        throw ExceptionWithLocation("Disc type not in available disc types: " + *invalidDiscTypeName);

    if (int totalPercent = calculateFrequencySum(discTypeDistribution); totalPercent != 100)
        throw ExceptionWithLocation("Percentages for disc type distribution don't add up to 100. They add up to " +
                                    std::to_string(totalPercent));

    settings_.discTypeDistribution_ = discTypeDistribution;

    useCallback(SettingID::DiscTypeDistribution);
}

void GlobalSettings::addReaction(const Reaction& reaction)
{
    throwIfLocked();

    throwIfNotInDiscTypes(*reaction.getEduct1(), settings_.discTypes_);
    throwIfNotInDiscTypes(*reaction.getProduct1(), settings_.discTypes_);

    if (reaction.hasEduct2())
        throwIfNotInDiscTypes(*reaction.getEduct2(), settings_.discTypes_);

    if (reaction.hasProduct2())
        throwIfNotInDiscTypes(*reaction.getProduct2(), settings_.discTypes_);

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

/* void GlobalSettings::loadFromJson(const fs::path& jsonFile)
{
    std::ifstream in(jsonFile);
    if (!in)
        throw ExceptionWithLocation("Couldn't open file: " + jsonFile.string());

    json j;
    in >> j;
    settings_ = j.get<Settings>();

    useCallback(SettingID::DiscTypeDistribution);
    useCallback(SettingID::Reactions);
    useCallback(SettingID::SimulationTimeScale);
    useCallback(SettingID::SimulationTimeStep);
}

void GlobalSettings::saveAsJson(const fs::path& jsonFile)
{
    std::ofstream out(jsonFile);
    if (!out)
        throw ExceptionWithLocation("Couldn't open file for writing: " + jsonFile.string());

    json j = settings_;
    out << j.dump(4);
}
 */
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

void GlobalSettings::useCallback(const SettingID& settingID)
{
    if (callback_)
        callback_(settingID);
}

} // namespace cell