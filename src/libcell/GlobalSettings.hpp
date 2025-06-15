#ifndef GLOBALSETTINGS_HPP
#define GLOBALSETTINGS_HPP

#include "DiscType.hpp"
#include "ExceptionWithLocation.hpp"
#include "Settings.hpp"
#include "StringUtils.hpp"

#include <SFML/System/Time.hpp>

#include <filesystem>
#include <functional>
#include <map>
#include <stdexcept>
#include <string>

namespace fs = std::filesystem;

namespace cell
{

/**
 * @brief Unique identifier of available settings. This way the callback for GlobalSettings can inform on which setting
 * was changed
 */
enum SettingID
{
    SimulationTimeStep = 1 << 0,
    SimulationTimeScale = 1 << 1,
    NumberOfDiscs = 1 << 2,
    DiscTypeDistribution = 1 << 3,
    Reactions = 1 << 4
};

/**
 * @brief Singleton for global simulation settings. See Settings.hpp for a detailed documentation of the different
 * settings
 */
class GlobalSettings
{
public:
    /**
     * @brief Global accessor for the singleton
     */
    static GlobalSettings& get();

    /**
     * @returns The currently set settings
     */
    static const Settings& getSettings();

    /**
     * @brief The custom callback will be called with the appropriate SettingID if a setting was changed
     * @note I use this for using the signal-slots-system of Qt without making the simulation-library Qt-dependent
     */
    static void setCallback(const std::function<void(const SettingID& settingID)>& functor);

    // All these setters check the limits in SettingsLimits, throw if locked and otherwise don't deserve their own
    // comment

    void setSimulationTimeStep(const sf::Time& simulationTimeStep);
    void setSimulationTimeScale(float simulationTimeScale);
    void setNumberOfDiscs(int numberOfDiscs);

    /**
     * @brief Sets the current disc type distribution and automatically removes all currently set reactions with
     * a DiscType as an educt or product that is not contained in the new distribution. Also updates all reactions if an
     * existent DiscType was changed. Validates that the distribution isn't empty and percentages add up to 100
     */
    void setDiscTypeDistribution(const DiscType::map<int>& discTypeDistribution);

    /**
     * @brief Determines the type of reaction and automatically adds it to the correct reaction map
     */
    void addReaction(const Reaction& reaction);

    /**
     * @brief Deletes all reactions from all reaction maps
     */
    void clearReactions();

    /**
     * @brief Prevent any changes to the settings
     */
    void lock();

    /**
     * @brief Enable changing of the settings again
     */
    void unlock();

    /**
     * @returns Current lock state of the settings (typically, settings are locked during simulation)
     */
    bool isLocked() const;

    /**
     * @brief Returns a vector containing all reactions in the reaction table
     */
    const std::vector<Reaction>& getReactions() const;

    /**
     * @brief Attempts to read in an instance of `Settings` from the json file and sets the settings to that instance
     */
    void loadFromJson(const fs::path& jsonFile);

    /**
     * @brief Saves the current `Settings` instance to the given json path
     */
    void saveAsJson(const fs::path& jsonFile);

private:
    /**
     * @brief Singleton: Private ctor
     */
    GlobalSettings();

    /**
     * @brief Helper function to remove reactions were removed DiscTypes are in the educts or products ("dangling"
     * reactions)
     */
    void removeDanglingReactions(const DiscType::map<int>& newDiscTypeDistribution);

    /**
     * @brief Helper function to update reactions were updated DiscTypes are in the educts or products
     */
    void updateDiscTypesInReactions(const DiscType::map<int>& newDiscTypeDistribution);

    /**
     * @brief Calls the custom callback if it exists on the SettingID
     */
    void useCallback(const SettingID& settingID);

    /**
     * @brief Helper function that thros an exception if GlobalSettings were locked using lock()
     */
    void throwIfLocked();

private:
    /**
     * @brief Actual settings, see there for documentation of the values
     */
    Settings settings_;

    /**
     * @brief Helper to prevent changes of the settings during simulation
     */
    bool locked_ = false;

    /**
     * @brief The callback which is called whenever a setting was changed
     */
    std::function<void(const SettingID& settingID)> callback_;
};

/**
 * @brief Helper function for range checking
 * @note Example: `throwIfNotInRange(theAge, 0, 120, "age");`
 */
template <typename T> void throwIfNotInRange(const T& value, const T& min, const T& max, const std::string& valueName)
{
    using stringutils::toString;

    if (value < min || value > max)
        throw ExceptionWithLocation("Value for \"" + valueName + "\" out of range: Must be between \"" + toString(min) +
                                    "\" and \"" + toString(max) + "\", but is \"" + toString(value) + "\"");
}

} // namespace cell

#endif /* GLOBALSETTINGS_HPP */
