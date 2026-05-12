#ifndef F1160089_C2A5_45FA_AC16_370C293275DE_HPP
#define F1160089_C2A5_45FA_AC16_370C293275DE_HPP

#include "SimulationFactory.hpp"
#include "SimulationConfig.hpp"

#include <filesystem>
#include <chrono>
#include <thread>

namespace fs = std::filesystem;
namespace ch = std::chrono;

namespace cell
{
    
class SimulationRunner
{
public:
    struct PerformanceData 
    {
        double actualScale;
        ch::nanoseconds timePerWholeUpdate;
        ch::nanoseconds timePerSimulationUpdate;
    };
    
    struct SimulationStepData
    {
        std::vector<Disc> discs;
        DiscTypeMap<int> collisionCounts;
    };
    
public:
    void useConfigFile(const fs::path& configFile);
    void setOutFile(const fs::path& outFile);
    void setSimulationTime(const ch::duration<double>& simulationTime);
    void runSimulation();
    void stopSimulation();
    void setPerformanceDataCallback(void (*callback)(PerformanceData));
    void setSimulationStepDataCallback(void (*callback)(SimulationStepData));
    
private:
    void loop(std::stop_token stopToken);
    void sendPerformanceData(ch::steady_clock::time_point& start, int& updates, ch::duration<double>& simulationUpdateTime) const;
    void sendSimulationStepData();
    
private:
    SimulationFactory simulationFactory_;
    SimulationConfig simulationConfig_;
    std::jthread thread_;
    void (*performanceDataCallback_)(PerformanceData) = nullptr;
    void (*simulationStepDataCallback_)(SimulationStepData) = nullptr;
};
    
}

#endif /* F1160089_C2A5_45FA_AC16_370C293275DE_HPP */
