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
    struct Options
    {
        bool gatherExpensiveData =
            false; // Calculate velocity histogram, average kinetic energy etc. by iterating all discs in the simulation
    };

private:
    void setOptions(const Options& options);
    void setStorageInterval(const ch::duration<double>& storageInterval);
    void receivePerformanceData(SimulationRunner::PerformanceData data);
    void processSimulationData(SimulationFactory& simulationFactory);
    void clearDataPoint(DataPoint& dataPoint);

private:
    Options options_;
    ch::duration<double> storageInterval_ = ch::milliseconds{100};
    DataPoint dataPointForStorage_;
    std::vector<DataPoint> dataPoints_;
};

} // namespace cell

#endif /* A0298BEF_1AF7_44D4_A4ED_8921F9D116D7_HPP */
