#include "StringUtils.hpp"

#include <SFML/System/Time.hpp>

#include <stdexcept>
#include <string>

template <typename T>
void GlobalSettings::throwIfNotInRange(const T& value, const T& min, const T& max, const std::string& valueName)
{
    using StringUtils::toString;

    if (value < min || value > max)
        throw std::runtime_error("Value for \"" + valueName + "\" out of range: Must be between \"" + toString(min) +
                                 "\" and \"" + toString(max) + "\", but is \"" + toString(value) + "\"");
}