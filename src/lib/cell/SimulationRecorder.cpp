#include "SimulationRecorder.hpp"
#include "Cell.hpp"

namespace cell
{
void SimulationRecorder::setOptions(const Options& options)
{
    options_ = options;
}

void SimulationRecorder::setStorageInterval(const ch::duration<double>& storageInterval)
{
    storageInterval_ = storageInterval;
}

void SimulationRecorder::receivePerformanceData(SimulationRunner::PerformanceData data)
{
}

void SimulationRecorder::processSimulationData(SimulationFactory& simulationFactory)
{
    if (!simulationFactory.cellIsBuilt())
        return;

    const auto& cell = simulationFactory.getCell();

    std::vector<const Compartment*> compartments({&cell});
    while (!compartments.empty())
    {
        const Compartment* compartment = compartments.back();
        compartments.pop_back();
        simulationStepData.discs.insert(simulationStepData.discs.end(), compartment->getDiscs().begin(),
                                        compartment->getDiscs().end());

        for (const auto& subCompartment : compartment->getCompartments())
            compartments.push_back(subCompartment.get());
    }

    simulationStepData.collisionCounts = simulationFactory.getAndResetCollisionCounts();
    postUpdateCallback_(std::move(simulationStepData));
}

} // namespace cell