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
    auto simulationConfig = j["config"].get<SimulationConfig>();
    simulationFactory_.buildSimulationFromConfig(simulationConfig);
}

void SimulationRunner::setSimulationDuration(const std::chrono::duration<double>& simulationDuration)
{
    simulationDuration_ = simulationDuration;
}

void SimulationRunner::runSimulation()
{
    thread_ = std::jthread([this](std::stop_token stopToken) { loop(stopToken); });
}

void SimulationRunner::waitForSimulationToFinish()
{
    if (thread_.joinable())
        thread_.join();
}

void SimulationRunner::stopSimulation()
{
    if (thread_.joinable())
        thread_.request_stop();
}

void SimulationRunner::setPerformanceDataCallback(std::function<void(PerformanceData)> callback)
{
    performanceDataCallback_ = callback;
}

void SimulationRunner::setPostUpdateCallback(
    std::function<void(Cell&, const SimulationContext&, const ch::duration<double>&)> callback)
{
    postUpdateCallback_ = callback;
}

void SimulationRunner::loop(std::stop_token stopToken)
{
    auto start = ch::steady_clock::now();
    auto lastUpdate = start;
    auto timeSinceLastUpdate = ch::duration<double>(0s);
    auto simulationUpdateTime = ch::duration<double>(0s);
    auto simulationDuration = ch::duration<double>(0s);
    const auto simulationTimeStep = ch::duration<double>(simulationConfig_.simulationTimeStep);
    int updates = 0;
    const auto simulationContext = simulationFactory_.getSimulationContext();

    while (!stopToken.stop_requested())
    {
        auto now = ch::steady_clock::now();
        timeSinceLastUpdate += now - lastUpdate;
        lastUpdate = now;

        sendPerformanceData(start, updates, simulationUpdateTime);

        while (timeSinceLastUpdate / simulationConfig_.simulationTimeScale > simulationTimeStep &&
               simulationDuration < simulationDuration_ && !stopToken.stop_requested())
        {
            timeSinceLastUpdate -= simulationTimeStep / simulationConfig_.simulationTimeScale;

            const auto updateStart = ch::steady_clock::now();
            simulationFactory_.getCell().update(simulationTimeStep.count());
            const auto elapsed = ch::steady_clock::now() - updateStart;
            simulationUpdateTime += elapsed;
            simulationDuration += elapsed;
            ++updates;

            sendPerformanceData(start, updates, simulationUpdateTime);

            if (postUpdateCallback_)
                postUpdateCallback_(simulationFactory_.getCell(), simulationContext, simulationTimeStep);
        }
    }
}

void SimulationRunner::sendPerformanceData(ch::steady_clock::time_point& start, int& updates,
                                           ch::duration<double>& simulationUpdateTime) const
{
    if (!performanceDataCallback_)
        return;

    const auto elapsed = start - ch::steady_clock::now();

    if (elapsed < 1s || updates == 0)
        return;

    const double simulationTime = updates * simulationConfig_.simulationTimeStep;
    const double elapsedSeconds = ch::duration<double>(elapsed).count();
    const double actualScale = simulationTime / elapsedSeconds;
    const auto timePerWholeUpdate = ch::duration_cast<ch::nanoseconds>(elapsed / updates);
    const auto timePerSimulationUpdate = ch::duration_cast<ch::nanoseconds>(simulationUpdateTime / updates);

    performanceDataCallback_(PerformanceData{.actualScale = actualScale,
                                             .timePerWholeUpdate = timePerWholeUpdate,
                                             .timePerSimulationUpdate = timePerSimulationUpdate});

    start = ch::steady_clock::now();
    updates = 0;
    simulationUpdateTime = 0s;
}

} // namespace cell