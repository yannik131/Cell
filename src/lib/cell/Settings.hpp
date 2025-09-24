#ifndef B6F87E72_2540_4C8C_89E9_E282B0972C52_HPP
#define B6F87E72_2540_4C8C_89E9_E282B0972C52_HPP

#include "ExceptionWithLocation.hpp"
#include "StringUtils.hpp"

#include <SFML/System/Time.hpp>

#include <map>
#include <utility>
#include <vector>

namespace cell
{

namespace SettingsLimits
{
extern const sf::Time MinSimulationTimeStep;
extern const sf::Time MaxSimulationTimeStep;

extern const double MinSimulationTimeScale;
extern const double MaxSimulationTimeScale;

extern const int MinNumberOfDiscs;
extern const int MaxNumberOfDiscs;

extern const double MinCellWidth;
extern const double MaxCellWidth;

extern const double MinCellHeight;
extern const double MaxCellHeight;

extern const double MinMaxVelocity;
extern const double MaxMaxVelocity;
} // namespace SettingsLimits

namespace DiscTypeLimits
{
extern const double MinRadius;
extern const double MaxRadius;

extern const double MinMass;
extern const double MaxMass;
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

#endif /* B6F87E72_2540_4C8C_89E9_E282B0972C52_HPP */
