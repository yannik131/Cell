#include "StringUtils.hpp"

#include <vector>

namespace cell
{

namespace stringutils
{

std::string timeString(long long timeNs)
{
    const static std::vector<std::string> timeUnits = {"ns", "us", "ms", "s"};

    auto convertedTime = static_cast<long double>(timeNs);
    std::size_t i = 0;
    while (convertedTime > 1e3 && i < timeUnits.size())
    {
        convertedTime /= 1e3;
        ++i;
    }

    return std::to_string(convertedTime) + timeUnits[i];
}

} // namespace stringutils

} // namespace cell