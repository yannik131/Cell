#ifndef E54887CD_F652_4F07_8CCB_961E814F1517_HPP
#define E54887CD_F652_4F07_8CCB_961E814F1517_HPP

#include <SFML/System/Time.hpp>
#include <string>

namespace cell::stringutils
{

/**
 * @brief Creates a string with the passed time in a sensible unit
 */
std::string timeString(long long timeNs);

/**
 * @brief Fall-back to std::string
 */
template <typename T> std::string toString(const T& value)
{
    return std::to_string(value);
}

/**
 * @brief Specialization for sf::Time
 */
template <> inline std::string toString<sf::Time>(const sf::Time& value)
{
    return timeString(value.asMicroseconds() * 1000);
}

} // namespace cell::stringutils

#endif /* E54887CD_F652_4F07_8CCB_961E814F1517_HPP */
