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
    currentDataPoint_.initializeHistograms(discTypeIDs, vSigma);
}

void SimulationRecorder::setStorageInterval(const ch::duration<double>& storageInterval)
{
    storageInterval_ = storageInterval;
}

void SimulationRecorder::printPerformanceData(SimulationRunner::PerformanceData data)
{
    std::cout << "Elapsed simulation time: " << data.elapsedSimulationTime.count() << "s\n";
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
    currentDataPoint_.addSimulationData(cell, ch::seconds{0}, discTypeRegistry_);
    recordFrame(cell);
}

void SimulationRecorder::processSimulationData(Cell& cell, const ch::duration<double>& elapsedTime)
{
    currentDataPoint_.addSimulationData(cell, elapsedTime, discTypeRegistry_);
    recordFrame(cell);
    storeDataPoint();
}

void SimulationRecorder::storeRemainingData()
{
    storeDataPoint();
}

const std::deque<DataPoint>& SimulationRecorder::getDataPoints() const
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

SimulationRecorder::Frame SimulationRecorder::getLastFrame()
{
    return lastFrame_;
}

void SimulationRecorder::setNewDataPointCallback(std::function<void(const DataPoint& dataPoint)> callback)
{
    newDataPointCallback_ = std::move(callback);
}

const ch::duration<double>& SimulationRecorder::getStorageInterval() const
{
    return storageInterval_;
}

void SimulationRecorder::storeDataPoint()
{
    if (currentDataPoint_.getData().elapsedTime < storageInterval_)
        return;

    currentDataPoint_.average(NormalizeCollisionCounts{false});
    dataPoints_.push_back(currentDataPoint_);

    if (newDataPointCallback_)
        newDataPointCallback_(currentDataPoint_);

    currentDataPoint_.clear();
}

void SimulationRecorder::recordFrame(const Cell& cell)
{
    if (!recordLastFrame_)
        return;

    lastFrame_.clear();

    std::vector<const Compartment*> compartments({&cell});
    while (!compartments.empty())
    {
        const Compartment* compartment = compartments.back();
        compartments.pop_back();

        lastFrame_.discs.insert(lastFrame_.discs.end(), compartment->getDiscs().begin(), compartment->getDiscs().end());
        lastFrame_.membranes.push_back(compartment->getMembrane());

        for (const auto& subCompartment : compartment->getCompartments())
            compartments.push_back(subCompartment.get());
    }
}

} // namespace cell