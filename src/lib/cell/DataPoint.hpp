#ifndef D33C2CD9_9240_4856_8DE0_B35E3611866C_HPP
#define D33C2CD9_9240_4856_8DE0_B35E3611866C_HPP

#include "Types.hpp"

#include <boost/histogram.hpp>

#include <chrono>
#include <string>
#include <unordered_map>

namespace ch = std::chrono;
namespace bh = boost::histogram;

namespace cell
{

using Histogram = bh::histogram<std::tuple<bh::axis::category<DiscTypeID>, bh::axis::regular<>>, bh::default_storage>;

struct NormalizeCollisionCounts
{
    bool value = true;
};

template <typename T>
void addMapToMap(std::unordered_map<DiscTypeID, double>& lhs, const std::unordered_map<DiscTypeID, T>& rhs)
{
    for (const auto& [key, value] : rhs)
        lhs[key] += value;
}

template <typename T> void divideMapByValue(std::unordered_map<DiscTypeID, double>& lhs, T rhs)
{
    for (auto& [key, value] : lhs)
        value /= rhs;
}

struct DataPoint
{
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
        addMapToMap(collisionCounts_, rhs.collisionCounts_);
        addMapToMap(totalKineticEnergies_, rhs.totalKineticEnergies_);
        addMapToMap(totalMomentums_, rhs.totalMomentums_);
        addMapToMap(discTypeCounts_, rhs.discTypeCounts_);
        vxHistogram_ += rhs.vxHistogram_;
        vyHistogram_ += rhs.vyHistogram_;
        vHistogram_ += rhs.vHistogram_;
    }

    void average(int n, NormalizeCollisionCounts normalizeCollisionCounts = {})
    {
        if (normalizeCollisionCounts.value && elapsedTime_.count() > 0)
            divideMapByValue(collisionCounts_, elapsedTime_.count());

        divideMapByValue(totalKineticEnergies_, n);
        divideMapByValue(totalMomentums_, n);
        divideMapByValue(discTypeCounts_, n);
        vxHistogram_ /= n;
        vyHistogram_ /= n;
        vHistogram_ /= n;
    }

    ch::duration<double> elapsedTime_;
    std::unordered_map<DiscTypeID, double> collisionCounts_;
    std::unordered_map<DiscTypeID, double> totalMomentums_;
    std::unordered_map<DiscTypeID, double> totalKineticEnergies_;
    std::unordered_map<DiscTypeID, double> discTypeCounts_;
    Histogram vxHistogram_;
    Histogram vyHistogram_;
    Histogram vHistogram_;
};

} // namespace cell

#endif /* D33C2CD9_9240_4856_8DE0_B35E3611866C_HPP */
