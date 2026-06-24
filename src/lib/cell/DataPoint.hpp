#ifndef D33C2CD9_9240_4856_8DE0_B35E3611866C_HPP
#define D33C2CD9_9240_4856_8DE0_B35E3611866C_HPP

#include "Cell.hpp"
#include "CollisionDetector.hpp"
#include "MathUtils.hpp"
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

class DataPoint
{
public:
    struct Data
    {
        ch::nanoseconds elapsedTime = {};
        std::unordered_map<DiscTypeID, double> collisionCounts;
        std::unordered_map<DiscTypeID, double> totalMomentums;
        std::unordered_map<DiscTypeID, double> totalKineticEnergies;
        std::unordered_map<DiscTypeID, double> discTypeCounts;
        Histogram vxHistogram;
        Histogram vyHistogram;
        Histogram vHistogram;
    };

public:
    const Data& getData() const;
    void clear();
    void add(const DataPoint& rhs);
    void average(NormalizeCollisionCounts normalizeCollisionCounts = {});
    void initializeHistograms(const std::vector<DiscTypeID>& discTypeIDs, double vSigma);
    void addSimulationData(Cell& cell, const ch::nanoseconds& elapsedTime, const DiscTypeRegistry& discTypeRegistry);

private:
    Data data_;
    int n_ = 0;
};

} // namespace cell

#endif /* D33C2CD9_9240_4856_8DE0_B35E3611866C_HPP */
