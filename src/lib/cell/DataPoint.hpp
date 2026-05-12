#ifndef D33C2CD9_9240_4856_8DE0_B35E3611866C_HPP
#define D33C2CD9_9240_4856_8DE0_B35E3611866C_HPP

#include <chrono>
#include <string>
#include <unordered_map>

namespace ch = std::chrono;

namespace cell
{

struct DataPoint
{
    ch::duration<double> elapsedTime;
    std::unordered_map<std::string, double> collisionCounts;

    void clear()
    {
        elapsedTime = ch::seconds{0};
        collisionCounts.clear();
    }

    void add(const DataPoint& rhs)
    {
        elapsedTime += rhs.elapsedTime;
        for (const auto& [key, value] : rhs.collisionCounts)
            collisionCounts[key] += value;
    }

    void average(int n)
    {
        for (const auto& [key, value] : collisionCounts)
            collisionCounts[key] /= n;
    }
};

} // namespace cell

#endif /* D33C2CD9_9240_4856_8DE0_B35E3611866C_HPP */
