#include "StringUtils.hpp"

#include <vector>

namespace cell::stringutils
{

std::string timeString(long long timeNs, int digits)
{
    static const std::vector<std::string> units = {"ns", "us", "ms", "s"};

    long double value = static_cast<long double>(timeNs);
    std::size_t i = 0;
    while (value >= 1e3 && i + 1 < units.size())
    {
        value /= 1e3;
        ++i;
    }

    std::ostringstream oss;
    if (digits >= 0)
        oss << std::fixed << std::setprecision(digits);

    oss << value << units[i];
    return oss.str();
}

} // namespace cell::stringutils