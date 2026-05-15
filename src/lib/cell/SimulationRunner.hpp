#ifndef F1160089_C2A5_45FA_AC16_370C293275DE_HPP
#define F1160089_C2A5_45FA_AC16_370C293275DE_HPP

#include "SimulationConfig.hpp"
#include "SimulationFactory.hpp"

#include <chrono>
#include <filesystem>
#include <thread>

namespace fs = std::filesystem;
namespace ch = std::chrono;

namespace cell
{

struct Force
{
    bool value = false;
};

class SimulationRunner
{
public:
    struct PerformanceData
    {
        double actualScale;
        ch::duration<double> timePerWholeUpdate;
        ch::duration<double> timePerSimulationUpdate;
    };

public:
    void useConfigFile(const fs::path& configFile);
    void setSimulationDuration(const ch::duration<double>& simulationDuration);
    void runSimulation();
    void waitForSimulationToFinish();
    void stopSimulation();
    void setPerformanceDataCallback(std::function<void(PerformanceData)> callback);
    void setPostUpdateCallback(std::function<void(Cell&, const ch::duration<double>&)> callback);
    SimulationContext getSimulationContext();
    const SimulationConfig& getSimulationConfig() const;

private:
    void loop(std::stop_token stopToken);
    void sendPerformanceData(ch::steady_clock::time_point& start, int& updates,
                             ch::duration<double>& simulationUpdateTime, Force force = {}) const;

private:
    SimulationFactory simulationFactory_;
    SimulationConfig simulationConfig_;
    std::jthread thread_;
    std::function<void(PerformanceData)> performanceDataCallback_;
    std::function<void(Cell&, const ch::duration<double>&)> postUpdateCallback_;
    ch::duration<double> simulationDuration_ = ch::duration<double>::max();
};

} // namespace cell

#endif /* F1160089_C2A5_45FA_AC16_370C293275DE_HPP */
