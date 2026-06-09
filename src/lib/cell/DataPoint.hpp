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
        ch::duration<double> elapsedTime = {};
        std::unordered_map<DiscTypeID, double> collisionCounts;
        std::unordered_map<DiscTypeID, double> totalMomentums;
        std::unordered_map<DiscTypeID, double> totalKineticEnergies;
        std::unordered_map<DiscTypeID, double> discTypeCounts;
        Histogram vxHistogram;
        Histogram vyHistogram;
        Histogram vHistogram;
    };

public:
    const Data& getData() const
    {
        return data_;
    }

    void clear()
    {
        data_.elapsedTime = ch::seconds{0};
        data_.collisionCounts.clear();
        data_.totalKineticEnergies.clear();
        data_.totalMomentums.clear();
        data_.discTypeCounts.clear();
        data_.vxHistogram.reset();
        data_.vyHistogram.reset();
        data_.vHistogram.reset();
        n_ = 0;
    }

    void add(const DataPoint& rhs)
    {
        data_.elapsedTime += rhs.data_.elapsedTime;
        addMapToMap(data_.collisionCounts, rhs.data_.collisionCounts);
        addMapToMap(data_.totalKineticEnergies, rhs.data_.totalKineticEnergies);
        addMapToMap(data_.totalMomentums, rhs.data_.totalMomentums);
        addMapToMap(data_.discTypeCounts, rhs.data_.discTypeCounts);
        data_.vxHistogram += rhs.data_.vxHistogram;
        data_.vyHistogram += rhs.data_.vyHistogram;
        data_.vHistogram += rhs.data_.vHistogram;
        n_ += rhs.n_;
    }

    void average(NormalizeCollisionCounts normalizeCollisionCounts = {})
    {
        if (n_ == 0)
            return;

        if (normalizeCollisionCounts.value && data_.elapsedTime.count() > 0)
            divideMapByValue(data_.collisionCounts, data_.elapsedTime.count());

        divideMapByValue(data_.totalKineticEnergies, n_);
        divideMapByValue(data_.totalMomentums, n_);
        divideMapByValue(data_.discTypeCounts, n_);
        data_.vxHistogram /= n_;
        data_.vyHistogram /= n_;
        data_.vHistogram /= n_;
        n_ = 1;
    }

    void initializeHistograms(const std::vector<DiscTypeID>& discTypeIDs, double vSigma)
    {
        data_.vxHistogram = bh::make_histogram(bh::axis::category<DiscTypeID>(discTypeIDs, "Disc type"),
                                               bh::axis::regular<>(20, -3 * vSigma, 3 * vSigma, "v_x"));

        data_.vyHistogram = bh::make_histogram(bh::axis::category<DiscTypeID>(discTypeIDs, "Disc type"),
                                               bh::axis::regular<>(20, -3 * vSigma, 3 * vSigma, "v_y"));

        data_.vHistogram = bh::make_histogram(bh::axis::category<DiscTypeID>(discTypeIDs, "Disc type"),
                                              bh::axis::regular<>(20, -3 * vSigma, 3 * vSigma, "v"));
    }

    void addSimulationData(Cell& cell, const ch::duration<double>& elapsedTime,
                           const DiscTypeRegistry& discTypeRegistry)
    {
        addMapToMap(data_.collisionCounts, CollisionDetector::getAndResetCollisionCounts());
        data_.elapsedTime += elapsedTime;

        std::unordered_map<DiscTypeID, cell::Vector2d> momentumMap;
        std::vector<const Compartment*> compartments({&cell});
        while (!compartments.empty())
        {
            const Compartment* compartment = compartments.back();
            compartments.pop_back();

            for (const auto& disc : compartment->getDiscs())
            {
                const auto discTypeID = disc.getTypeID();
                const auto mass = discTypeRegistry.getByID(disc.getTypeID()).getMass();

                ++data_.discTypeCounts[discTypeID];
                data_.totalKineticEnergies[discTypeID] += disc.getKineticEnergy(mass);
                momentumMap[discTypeID] += disc.getMomentum(mass);
                data_.vxHistogram(discTypeID, disc.getVelocity().x);
                data_.vyHistogram(discTypeID, disc.getVelocity().y);
                data_.vHistogram(discTypeID, mathutils::abs(disc.getVelocity()));
            }

            for (const auto& subCompartment : compartment->getCompartments())
                compartments.push_back(subCompartment.get());
        }

        for (const auto& [discTypeID, momentum] : momentumMap)
            data_.totalMomentums[discTypeID] = mathutils::abs(momentum);

        ++n_;
    }

private:
    Data data_;
    int n_ = 0;
};

} // namespace cell

#endif /* D33C2CD9_9240_4856_8DE0_B35E3611866C_HPP */
