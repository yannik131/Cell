#ifndef A0298BEF_1AF7_44D4_A4ED_8921F9D116D7_HPP
#define A0298BEF_1AF7_44D4_A4ED_8921F9D116D7_HPP

#include "DataPoint.hpp"
#include "SimulationRunner.hpp"

#include <boost/histogram.hpp>

namespace cell
{

class SimulationRecorder
{
public:
    void setStorageInterval(const ch::duration<double>& storageInterval);
    void receivePerformanceData(SimulationRunner::PerformanceData data);
    void processSimulationData(Cell& cell, const SimulationContext& simulationContext,
                               const ch::duration<double>& elapsedTime);
    void storeRemainingData();
    const std::vector<DataPoint>& getDataPoints() const;

private:
    void addSimulationDataToDataPoint(Cell& cell, const SimulationContext& simulationContext,
                                      const ch::duration<double>& elapsedTime);
    void storeDataPoint();

private:
    ch::duration<double> storageInterval_ = ch::milliseconds{100};
    DataPoint dataPointForStorage_;
    std::vector<DataPoint> dataPoints_;
    int frameCount_ = 0;
};

} // namespace cell

#endif /* A0298BEF_1AF7_44D4_A4ED_8921F9D116D7_HPP */
