#include "SimulationRecorder.hpp"
#include "Cell.hpp"
#include "MathUtils.hpp"
#include "StringUtils.hpp"

#include <iostream>

namespace cell
{

SimulationRecorder::SimulationRecorder(const DiscTypeRegistry& discTypeRegistry,
                                       const SimulationConfig& simulationConfig)
    : discTypeRegistry_(&discTypeRegistry)
{
    std::vector<DiscTypeID> discTypeIDs = discTypeRegistry.getIDs();
    const double sigma = simulationConfig.mostProbableSpeed;

    dataPointForStorage_.vxHistogram_ = bh::make_histogram(bh::axis::category<DiscTypeID>(discTypeIDs, "Disc type"),
                                                           bh::axis::regular<>(20, -3 * sigma, 3 * sigma, "v_x"));

    dataPointForStorage_.vyHistogram_ = bh::make_histogram(bh::axis::category<DiscTypeID>(discTypeIDs, "Disc type"),
                                                           bh::axis::regular<>(20, -3 * sigma, 3 * sigma, "v_y"));

    dataPointForStorage_.vHistogram_ = bh::make_histogram(bh::axis::category<DiscTypeID>(discTypeIDs, "Disc type"),
                                                          bh::axis::regular<>(20, -3 * sigma, 3 * sigma, "v"));
}

void SimulationRecorder::setStorageInterval(const ch::duration<double>& storageInterval)
{
    storageInterval_ = storageInterval;
}

void SimulationRecorder::receivePerformanceData(SimulationRunner::PerformanceData data)
{
    std::cout << "Actual scale: " << data.actualScale << "\n";
    std::cout << "Time per simulation update: "
              << stringutils::timeString(ch::duration_cast<ch::nanoseconds>(data.timePerSimulationUpdate).count())
              << "\n";
    std::cout << "Time per update: "
              << stringutils::timeString(ch::duration_cast<ch::nanoseconds>(data.timePerWholeUpdate).count()) << "\n";
    std::cout << std::endl;
}

void SimulationRecorder::processSimulationData(Cell& cell, const ch::duration<double>& elapsedTime)
{
    addSimulationDataToDataPoint(cell, elapsedTime);
    if (dataPointForStorage_.elapsedTime_ >= storageInterval_)
        storeDataPoint();
}

void SimulationRecorder::storeRemainingData()
{
    if (frameCount_ == 0)
        return;

    storeDataPoint();
}

const std::vector<DataPoint>& SimulationRecorder::getDataPoints() const
{
    return dataPoints_;
}

void SimulationRecorder::addSimulationDataToDataPoint(Cell& cell, const ch::duration<double>& elapsedTime)
{
    addMapToMap(dataPointForStorage_.collisionCounts_, CollisionDetector::getAndResetCollisionCounts());
    dataPointForStorage_.elapsedTime_ += elapsedTime;

    std::unordered_map<DiscTypeID, cell::Vector2d> momentumMap;
    std::vector<const Compartment*> compartments({&cell});
    while (!compartments.empty())
    {
        const Compartment* compartment = compartments.back();
        compartments.pop_back();

        for (const auto& disc : compartment->getDiscs())
        {
            const auto discTypeID = disc.getTypeID();
            const auto mass = discTypeRegistry_->getByID(disc.getTypeID()).getMass();

            ++dataPointForStorage_.discTypeCounts_[discTypeID];
            dataPointForStorage_.totalKineticEnergies_[discTypeID] += disc.getKineticEnergy(mass);
            momentumMap[discTypeID] += disc.getMomentum(mass);
            dataPointForStorage_.vxHistogram_(discTypeID, disc.getVelocity().x);
            dataPointForStorage_.vyHistogram_(discTypeID, disc.getVelocity().y);
            dataPointForStorage_.vHistogram_(discTypeID, mathutils::abs(disc.getVelocity()));
        }

        for (const auto& subCompartment : compartment->getCompartments())
            compartments.push_back(subCompartment.get());
    }

    for (const auto& [discTypeID, momentum] : momentumMap)
        dataPointForStorage_.totalMomentums_[discTypeID] = mathutils::abs(momentum);

    ++frameCount_;
}

void SimulationRecorder::storeDataPoint()
{
    dataPointForStorage_.average(frameCount_, NormalizeCollisionCounts{false});
    dataPoints_.push_back(dataPointForStorage_);
    dataPointForStorage_.clear();
    frameCount_ = 0;
}

} // namespace cell