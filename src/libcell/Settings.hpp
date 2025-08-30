#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include "ExceptionWithLocation.hpp"
#include "StringUtils.hpp"

#include <SFML/System/Time.hpp>
#include <nlohmann/json.hpp>

#include <map>
#include <utility>
#include <vector>

namespace cell
{

/* NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Settings, simulationTimeStep_, simulationTimeScale_, numberOfDiscs_, cellWidth_,
                                   cellHeight_, discTypeDistribution_, discTypes_, reactionTable_) */

namespace SettingsLimits
{
const sf::Time MinSimulationTimeStep = sf::microseconds(1);
const sf::Time MaxSimulationTimeStep = sf::milliseconds(100);

const double MinSimulationTimeScale = 0.0001f;
const double MaxSimulationTimeScale = 10.0;

const int MinNumberOfDiscs = 1;
const int MaxNumberOfDiscs = 10000;

const int MinCellWidth = 100;
const int MaxCellWidth = 100000;

const int MinCellHeight = 100;
const int MaxCellHeight = 100000;
} // namespace SettingsLimits

namespace DiscTypeLimits
{
const double MinRadius = 1.0;
const double MaxRadius = 100.0;

const double MinMass = 1.0;
const double MaxMass = 10000.0;
} // namespace DiscTypeLimits

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

#endif /* SETTINGS_HPP */