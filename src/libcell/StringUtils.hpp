#ifndef STRINGUTILS_HPP
#define STRINGUTILS_HPP

#include <SFML/System/Time.hpp>
#include <string>

namespace StringUtils
{

std::string timeString(long long timeNs);

template <typename T> std::string toString(const T& value)
{
    return std::to_string(value);
}

template <> inline std::string toString<sf::Time>(const sf::Time& value)
{
    return timeString(value.asMicroseconds() * 1000);
}

} // namespace StringUtils

#endif /* STRINGUTILS_HPP */
