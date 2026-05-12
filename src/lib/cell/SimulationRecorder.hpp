#ifndef A0298BEF_1AF7_44D4_A4ED_8921F9D116D7_HPP
#define A0298BEF_1AF7_44D4_A4ED_8921F9D116D7_HPP

#include "SimulationRunner.hpp"

namespace cell
{

class SimulationRecorder
{
public:
    void setStorageInterval(const ch::duration<double>& storageInterval);
    void receivePerformanceData(SimulationRunner::PerformanceData data);
    void receiveSimulationStepData(SimulationRunner::SimulationStepData data);
};

} // namespace cell

#endif /* A0298BEF_1AF7_44D4_A4ED_8921F9D116D7_HPP */
