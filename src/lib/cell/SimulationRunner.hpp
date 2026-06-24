#ifndef F1160089_C2A5_45FA_AC16_370C293275DE_HPP
#define F1160089_C2A5_45FA_AC16_370C293275DE_HPP

#include "SimulationConfig.hpp"
#include "SimulationFactory.hpp"

#include <atomic>
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
        double targetScale;
        double actualScale;
        ch::nanoseconds timePerWholeUpdate;
        ch::nanoseconds timePerSimulationUpdate;
        ch::nanoseconds elapsedSimulationTime;
    };

    struct LoopParameters
    {
        double targetScale;
        ch::nanoseconds timeStep;
    };

public:
    void useConfigFile(const fs::path& configFile);
    void useConfig(const SimulationConfig& simulationConfig);
    void setSimulationDuration(const ch::nanoseconds& simulationDuration);
    void runSimulation();
    void waitForSimulationToFinish();
    void stopSimulation();
    void setPerformanceDataCallback(std::function<void(PerformanceData)> callback);
    void setPostBuildCallback(std::function<void(Cell&)> callback);
    void setPostUpdateCallback(std::function<void(Cell&, const ch::nanoseconds&)> callback);
    void setPostStartCallback(std::function<void()> callback);
    void setPostStopCallback(std::function<void()> callback);
    SimulationContext getSimulationContext() const;
    const SimulationConfig& getSimulationConfig() const;
    void setUseScaleFromConfig(bool value);
    bool simulationIsRunning() const;
    void updateLoopParameters(LoopParameters loopParameters);

private:
    void loop(std::stop_token stopToken);
    void sendPerformanceData(ch::steady_clock::time_point& start, int& updates, ch::nanoseconds& simulationUpdateTime,
                             const ch::nanoseconds& elapsedSimulationTime, Force force = {}) const;

private:
    SimulationFactory simulationFactory_;
    SimulationConfig simulationConfig_;
    std::jthread thread_;
    std::function<void(PerformanceData)> performanceDataCallback_;
    std::function<void(Cell&, const ch::nanoseconds&)> postUpdateCallback_;
    std::function<void(Cell&)> postBuildCallback_;
    std::function<void()> postStartCallback_;
    std::function<void()> postStopCallback_;
    ch::nanoseconds simulationDuration_ = ch::nanoseconds::max();
    bool useScaleFromConfig_ = false;
    std::atomic<bool> isRunning_ = false;
};

} // namespace cell

#endif /* F1160089_C2A5_45FA_AC16_370C293275DE_HPP */
