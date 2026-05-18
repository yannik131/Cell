#include "SimulationRecorder.hpp"
#include "Cell.hpp"
#include "MathUtils.hpp"
#include "StringUtils.hpp"

#include <iostream>

namespace cell
{

SimulationRecorder::SimulationRecorder(const DiscTypeRegistry& discTypeRegistry, double vSigma)
    : discTypeRegistry_(discTypeRegistry)
{
    std::vector<DiscTypeID> discTypeIDs = discTypeRegistry.getIDs();

    currentDataPoint_.vxHistogram_ = bh::make_histogram(bh::axis::category<DiscTypeID>(discTypeIDs, "Disc type"),
                                                        bh::axis::regular<>(20, -3 * vSigma, 3 * vSigma, "v_x"));

    currentDataPoint_.vyHistogram_ = bh::make_histogram(bh::axis::category<DiscTypeID>(discTypeIDs, "Disc type"),
                                                        bh::axis::regular<>(20, -3 * vSigma, 3 * vSigma, "v_y"));

    currentDataPoint_.vHistogram_ = bh::make_histogram(bh::axis::category<DiscTypeID>(discTypeIDs, "Disc type"),
                                                       bh::axis::regular<>(20, -3 * vSigma, 3 * vSigma, "v"));
}

void SimulationRecorder::setStorageInterval(const ch::duration<double>& storageInterval)
{
    storageInterval_ = storageInterval;
}

void SimulationRecorder::printPerformanceData(SimulationRunner::PerformanceData data)
{
    std::cout << "Actual scale: " << data.actualScale << "\n";
    std::cout << "Time per simulation update: "
              << stringutils::timeString(ch::duration_cast<ch::nanoseconds>(data.timePerSimulationUpdate).count())
              << "\n";
    std::cout << "Time per update: "
              << stringutils::timeString(ch::duration_cast<ch::nanoseconds>(data.timePerWholeUpdate).count()) << "\n";
    std::cout << std::endl;
}

void SimulationRecorder::processInitialSimulationData(Cell& cell)
{
    addSimulationDataToDataPoint(cell, ch::seconds{0});
}

void SimulationRecorder::processSimulationData(Cell& cell, const ch::duration<double>& elapsedTime)
{
    addSimulationDataToDataPoint(cell, elapsedTime);
    storeDataPoint();
}

void SimulationRecorder::storeRemainingData()
{
    storeDataPoint();
}

const std::vector<DataPoint>& SimulationRecorder::getDataPoints() const
{
    return dataPoints_;
}

void SimulationRecorder::clear()
{
    currentDataPoint_.clear();
    dataPoints_.clear();
}

const DataPoint& SimulationRecorder::getCurrentDataPoint() const
{
    return currentDataPoint_;
}

void SimulationRecorder::setRecordLastFrame(bool value)
{
    recordLastFrame_ = value;
}

const SimulationRecorder::Frame& SimulationRecorder::getLastFrame() const
{
    return lastFrame_;
}

void SimulationRecorder::setNewDataPointCallback(std::function<void(const DataPoint& dataPoint)> callback)
{
    newDataPointCallback_ = callback;
}

void SimulationRecorder::addSimulationDataToDataPoint(Cell& cell, const ch::duration<double>& elapsedTime)
{
    addMapToMap(currentDataPoint_.collisionCounts_, CollisionDetector::getAndResetCollisionCounts());
    currentDataPoint_.elapsedTime_ += elapsedTime;

    if (recordLastFrame_)
        lastFrame_.clear();

    std::unordered_map<DiscTypeID, cell::Vector2d> momentumMap;
    std::vector<const Compartment*> compartments({&cell});
    while (!compartments.empty())
    {
        const Compartment* compartment = compartments.back();
        compartments.pop_back();

        for (const auto& disc : compartment->getDiscs())
        {
            const auto discTypeID = disc.getTypeID();
            const auto mass = discTypeRegistry_.getByID(disc.getTypeID()).getMass();

            ++currentDataPoint_.discTypeCounts_[discTypeID];
            currentDataPoint_.totalKineticEnergies_[discTypeID] += disc.getKineticEnergy(mass);
            momentumMap[discTypeID] += disc.getMomentum(mass);
            currentDataPoint_.vxHistogram_(discTypeID, disc.getVelocity().x);
            currentDataPoint_.vyHistogram_(discTypeID, disc.getVelocity().y);
            currentDataPoint_.vHistogram_(discTypeID, mathutils::abs(disc.getVelocity()));
        }

        if (recordLastFrame_)
        {
            lastFrame_.discs.insert(lastFrame_.discs.end(), compartment->getDiscs().begin(),
                                    compartment->getDiscs().end());
            lastFrame_.membranes.push_back(compartment->getMembrane());
        }

        for (const auto& subCompartment : compartment->getCompartments())
            compartments.push_back(subCompartment.get());
    }

    for (const auto& [discTypeID, momentum] : momentumMap)
        currentDataPoint_.totalMomentums_[discTypeID] = mathutils::abs(momentum);

    ++frameCount_;
}

void SimulationRecorder::storeDataPoint()
{
    if (currentDataPoint_.elapsedTime_ < storageInterval_ || frameCount_ == 0)
        return;

    currentDataPoint_.average(frameCount_, NormalizeCollisionCounts{false});
    dataPoints_.push_back(currentDataPoint_);
    newDataPointCallback_(currentDataPoint_);
    currentDataPoint_.clear();
    frameCount_ = 0;
}

} // namespace cell