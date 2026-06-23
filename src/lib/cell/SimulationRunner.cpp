#include "SimulationRunner.hpp"
#include "Cell.hpp"

#include <fstream>

namespace ch = std::chrono;
using namespace std::chrono_literals;
using json = nlohmann::json;

namespace cell
{

void SimulationRunner::useConfigFile(const fs::path& configFile)
{
    json j;
    std::ifstream file(configFile);
    file >> j;
    simulationConfig_ = j["config"].get<SimulationConfig>();
    simulationFactory_.buildSimulationFromConfig(simulationConfig_);

    if (postBuildCallback_)
        postBuildCallback_(simulationFactory_.getCell());
}

void SimulationRunner::useConfig(const SimulationConfig& simulationConfig)
{
    if (simulationIsRunning())
        return;

    simulationFactory_.buildSimulationFromConfig(simulationConfig);
    simulationConfig_ = simulationConfig;

    if (postBuildCallback_)
        postBuildCallback_(simulationFactory_.getCell());
}

void SimulationRunner::setSimulationDuration(const std::chrono::duration<double>& simulationDuration)
{
    simulationDuration_ = simulationDuration;
}

void SimulationRunner::runSimulation()
{
    if (simulationIsRunning())
        return;

    thread_ = std::jthread([this](std::stop_token stopToken) { loop(stopToken); });
    isRunning_ = true;
}

void SimulationRunner::waitForSimulationToFinish()
{
    if (thread_.joinable() && simulationIsRunning())
        thread_.join();
}

void SimulationRunner::stopSimulation()
{
    if (thread_.joinable() && simulationIsRunning())
        thread_.request_stop();
}

void SimulationRunner::setPerformanceDataCallback(std::function<void(PerformanceData)> callback)
{
    performanceDataCallback_ = std::move(callback);
}

void SimulationRunner::setPostBuildCallback(std::function<void(Cell&)> callback)
{
    postBuildCallback_ = std::move(callback);

    if (simulationFactory_.cellIsBuilt() && postBuildCallback_)
        postBuildCallback_(simulationFactory_.getCell());
}

void SimulationRunner::setPostUpdateCallback(std::function<void(Cell&, const ch::duration<double>&)> callback)
{
    postUpdateCallback_ = std::move(callback);
}

void SimulationRunner::setPostStartCallback(std::function<void()> callback)
{
    postStartCallback_ = std::move(callback);

    if (simulationIsRunning() && postStartCallback_)
        postStartCallback_();
}

void SimulationRunner::setPostStopCallback(std::function<void()> callback)
{
    postStopCallback_ = std::move(callback);
}

SimulationContext SimulationRunner::getSimulationContext() const
{
    return simulationFactory_.getSimulationContext();
}

const SimulationConfig& SimulationRunner::getSimulationConfig() const
{
    return simulationConfig_;
}

void SimulationRunner::setUseScaleFromConfig(bool value)
{
    useScaleFromConfig_ = value;
}

bool SimulationRunner::simulationIsRunning() const
{
    return isRunning_;
}

void SimulationRunner::updateLoopParameters(LoopParameters loopParameters)
{
    // Not atomic atm, doesn't need to be yet
    simulationConfig_.simulationTimeScale = loopParameters.targetScale;
    simulationConfig_.simulationTimeStep = loopParameters.timeStep;
}

void SimulationRunner::loop(std::stop_token stopToken)
{
    if (postStartCallback_)
        postStartCallback_();

    auto simulationUpdateTime = ch::duration<double>(0s);
    auto simulationDuration = ch::duration<double>(0s);
    const auto simulationTimeStep = ch::duration<double>(simulationConfig_.simulationTimeStep);
    int updates = 0;
    auto start = ch::steady_clock::now();
    const ch::duration<double> realTimePerStep = simulationTimeStep / simulationConfig_.simulationTimeScale;
    auto nextTick = start;

    while (!stopToken.stop_requested() && simulationDuration < simulationDuration_)
    {
        const auto updateStart = ch::steady_clock::now();
        simulationFactory_.getCell().update(simulationTimeStep.count());
        const auto elapsed = ch::steady_clock::now() - updateStart;
        simulationUpdateTime += elapsed;
        simulationDuration += simulationTimeStep;
        ++updates;

        sendPerformanceData(start, updates, simulationUpdateTime, simulationDuration);

        if (postUpdateCallback_)
            postUpdateCallback_(simulationFactory_.getCell(), simulationTimeStep);

        if (useScaleFromConfig_)
        {
            nextTick += ch::duration_cast<ch::steady_clock::duration>(realTimePerStep);
            std::this_thread::sleep_until(nextTick);
        }
    }

    sendPerformanceData(start, updates, simulationUpdateTime, simulationDuration, Force{true});
    isRunning_ = false;

    if (postStopCallback_)
        postStopCallback_();
}

void SimulationRunner::sendPerformanceData(ch::steady_clock::time_point& start, int& updates,
                                           ch::duration<double>& simulationUpdateTime,
                                           const ch::duration<double>& elapsedSimulationTime, Force force) const
{
    if (!performanceDataCallback_)
        return;

    const auto elapsed = ch::steady_clock::now() - start;

    if (updates == 0 || (elapsed < 1s && !force.value))
        return;

    const double simulationTime = updates * simulationConfig_.simulationTimeStep;
    const double elapsedSeconds = ch::duration<double>(elapsed).count();
    const double actualScale = simulationTime / elapsedSeconds;
    const auto timePerWholeUpdate = elapsed / updates;
    const auto timePerSimulationUpdate = simulationUpdateTime / updates;

    performanceDataCallback_(PerformanceData{.targetScale = simulationConfig_.simulationTimeScale,
                                             .actualScale = actualScale,
                                             .timePerWholeUpdate = timePerWholeUpdate,
                                             .timePerSimulationUpdate = timePerSimulationUpdate,
                                             .elapsedSimulationTime = elapsedSimulationTime});

    start = ch::steady_clock::now();
    updates = 0;
    simulationUpdateTime = 0s;
}

} // namespace cell