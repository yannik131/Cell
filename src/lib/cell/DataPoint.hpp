#ifndef D33C2CD9_9240_4856_8DE0_B35E3611866C_HPP
#define D33C2CD9_9240_4856_8DE0_B35E3611866C_HPP

#include "Types.hpp"

#include <chrono>
#include <string>
#include <unordered_map>

namespace ch = std::chrono;
namespace bh = boost::histogram;

using Histogram = bh::histogram<std::tuple<bh::axis::category<std::string>, bh::axis::regular<>>, bh::default_storage>;

namespace cell
{

struct NormalizeCollisionCounts
{
    bool value = true;
};

std::unordered_map<std::string, double>& operator+=(std::unordered_map<std::string, double>& lhs,
                                                    const std::unordered_map<std::string, double>& rhs)
{
    for (const auto& [key, value] : rhs)
        lhs[key] += value;

    return lhs;
}

std::unordered_map<std::string, double>& operator/=(std::unordered_map<std::string, double>& lhs, int rhs)
{
    for (auto& [key, value] : lhs)
        value /= rhs;

    return lhs;
}

class DataPoint
{
public:
    void clear()
    {
        elapsedTime_ = ch::seconds{0};
        collisionCounts_.clear();
        totalKineticEnergies_.clear();
        totalMomentums_.clear();
        discTypeCounts_.clear();
        vxHistogram_.reset();
        vyHistogram_.reset();
        vHistogram_.reset();
    }

    void add(const DataPoint& rhs)
    {
        elapsedTime_ += rhs.elapsedTime_;
        collisionCounts_ += rhs.collisionCounts_;
        totalKineticEnergies_ += rhs.totalKineticEnergies_;
        totalMomentums_ += rhs.totalMomentums_;
        discTypeCounts_ += rhs.discTypeCounts_;
        vxHistogram_ += rhs.vxHistogram_;
        vyHistogram_ += rhs.vyHistogram_;
        vHistogram_ += rhs.vHistogram_;
    }

    void average(int n, NormalizeCollisionCounts normalizeCollisionCounts = {})
    {
        if (normalizeCollisionCounts.value && elapsedTime_.count() > 0)
            collisionCounts_ /= elapsedTime_.count();

        totalKineticEnergies_ /= n;
        totalMomentums_ /= n;
        discTypeCounts_ /= n;
        vxHistogram_ /= n;
        vyHistogram_ /= n;
        vHistogram_ /= n;
    }

private:
    ch::duration<double> elapsedTime_;
    std::unordered_map<std::string, double> collisionCounts_;
    std::unordered_map<std::string, double> totalMomentums_;
    std::unordered_map<std::string, double> totalKineticEnergies_;
    std::unordered_map<std::string, double> discTypeCounts_;
    Histogram vxHistogram_;
    Histogram vyHistogram_;
    Histogram vHistogram_;
};

} // namespace cell

#endif /* D33C2CD9_9240_4856_8DE0_B35E3611866C_HPP */
