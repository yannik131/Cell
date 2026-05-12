#include "SimulationRecorder.hpp"

namespace cell
{

void SimulationRecorder::setStorageInterval(const ch::duration<double>& storageInterval)
{
    storageInterval_ = storageInterval;
}

void SimulationRecorder::receivePerformanceData(SimulationRunner::PerformanceData data)
{
}

void SimulationRecorder::receiveSimulationStepData(SimulationRunner::SimulationStepData data)
{
}

}