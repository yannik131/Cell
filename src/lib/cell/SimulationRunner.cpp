#include "SimulationRunner.hpp"
#include "Cell.hpp"

#include <fstream>

namespace ch = std::chrono;
using namespace std::chrono_literals;
using json = nlohmann::json;

void cell::SimulationRunner::useConfigFile(const fs::path& configFile)
{
    json j;
    std::ifstream file(configFile);
    file >> j;
    auto simulationConfig = j["config"].get<SimulationConfig>();
    simulationFactory_.buildSimulationFromConfig(simulationConfig);
}

void cell::SimulationRunner::setSimulationDuration(const std::chrono::duration<double>& simulationDuration)
{
    simulationDuration_ = simulationDuration;
}

void cell::SimulationRunner::runSimulation()
{
    thread_ = std::jthread([this](std::stop_token stopToken) { loop(stopToken); });
}

void cell::SimulationRunner::waitForSimulationToFinish()
{
    if (thread_.joinable())
        thread_.join();
}

void cell::SimulationRunner::stopSimulation()
{
    if (thread_.joinable())
        thread_.request_stop();
}

void cell::SimulationRunner::setPerformanceDataCallback(std::function<void(PerformanceData)> callback)
{
    performanceDataCallback_ = callback;
}

void cell::SimulationRunner::setSimulationStepDataCallback(std::function<void(SimulationStepData)> callback)
{
    simulationStepDataCallback_ = callback;
}

void cell::SimulationRunner::loop(std::stop_token stopToken)
{
    auto start = ch::steady_clock::now();
    auto lastUpdate = start;
    auto timeSinceLastUpdate = ch::duration<double>(0s);
    auto simulationUpdateTime = ch::duration<double>(0s);
    auto simulationDuration = ch::duration<double>(0s);
    const auto simulationTimeStep = ch::duration<double>(simulationConfig_.simulationTimeStep);
    int updates = 0;

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
            sendSimulationStepData();
        }
    }
}

void cell::SimulationRunner::sendPerformanceData(ch::steady_clock::time_point& start, int& updates,
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

void cell::SimulationRunner::sendSimulationStepData()
{
    if (!simulationFactory_.cellIsBuilt() || !simulationStepDataCallback_)
        return;

    SimulationStepData simulationStepData;
    const auto& cell = simulationFactory_.getCell();

    std::vector<const cell::Compartment*> compartments({&cell});
    while (!compartments.empty())
    {
        const cell::Compartment* compartment = compartments.back();
        compartments.pop_back();
        simulationStepData.discs.insert(simulationStepData.discs.end(), compartment->getDiscs().begin(),
                                        compartment->getDiscs().end());

        for (const auto& subCompartment : compartment->getCompartments())
            compartments.push_back(subCompartment.get());
    }

    simulationStepData.collisionCounts = simulationFactory_.getAndResetCollisionCounts();
    simulationStepDataCallback_(std::move(simulationStepData));
}
