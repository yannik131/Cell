#ifndef E54887CD_F652_4F07_8CCB_961E814F1517_HPP
#define E54887CD_F652_4F07_8CCB_961E814F1517_HPP

#include "MathUtils.hpp"
#include "Vector2d.hpp"

#include <sstream>
#include <string>

namespace cell::stringutils
{

/**
 * @brief Creates a string with the passed time in a sensible unit
 */
std::string timeString(long long timeNs, int digits = -1);

/**
 * @brief Fall-back to std::string
 */
template <typename T> std::string toString(const T& value)
{
    return std::to_string(value);
}

template <> inline std::string toString<std::chrono::nanoseconds>(const std::chrono::nanoseconds& value)
{
    return timeString(value.count());
}

template <> inline std::string toString<Vector2d>(const Vector2d& value)
{
    std::ostringstream oss;
    oss << value;

    return oss.str();
}

} // namespace cell::stringutils

#endif /* E54887CD_F652_4F07_8CCB_961E814F1517_HPP */
