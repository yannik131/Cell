#ifndef A0298BEF_1AF7_44D4_A4ED_8921F9D116D7_HPP
#define A0298BEF_1AF7_44D4_A4ED_8921F9D116D7_HPP

#include "DataPoint.hpp"
#include "Disc.hpp"
#include "Membrane.hpp"
#include "SimulationRunner.hpp"

#include <boost/histogram.hpp>

#include <deque>

namespace cell
{

class SimulationRecorder
{
public:
    struct Frame
    {
        std::vector<Disc> discs;
        std::vector<Membrane> membranes;

        void clear()
        {
            discs.clear();
            membranes.clear();
        }
    };

public:
    SimulationRecorder(const DiscTypeRegistry& discTypeRegistry, double vSigma);
    void setStorageInterval(const ch::duration<double>& storageInterval);
    void printPerformanceData(SimulationRunner::PerformanceData data);
    void processInitialSimulationData(Cell& cell);
    void processSimulationData(Cell& cell, const ch::duration<double>& elapsedTime);
    void storeRemainingData();
    const std::deque<DataPoint>& getDataPoints() const;
    void clear();
    const DataPoint& getCurrentDataPoint() const;
    void setRecordLastFrame(bool value);
    Frame getLastFrame();
    void setNewDataPointCallback(std::function<void(const DataPoint& dataPoint)> callback);
    const ch::duration<double>& getStorageInterval() const;

private:
    void storeDataPoint();
    void recordFrame(const Cell& cell);

private:
    ch::duration<double> storageInterval_ = ch::milliseconds{100};
    DataPoint currentDataPoint_;
    std::deque<DataPoint> dataPoints_;
    const DiscTypeRegistry& discTypeRegistry_;
    bool recordLastFrame_ = false;
    Frame lastFrame_;
    std::function<void(const DataPoint&)> newDataPointCallback_;
};

} // namespace cell

#endif /* A0298BEF_1AF7_44D4_A4ED_8921F9D116D7_HPP */
