#include "SimulationRunner.hpp"
#include "Cell.hpp"

namespace ch = std::chrono;
using namespace std::chrono_literals;

void cell::SimulationRunner::useConfigFile(const fs::path& configFile)
{
}

void cell::SimulationRunner::setOutFile(const fs::path& outFile)
{
}

void cell::SimulationRunner::setSimulationTime(const std::chrono::duration<double>& simulationTime)
{
}

void cell::SimulationRunner::runSimulation()
{
    thread_ = std::jthread([this](std::stop_token stopToken) { loop(stopToken); });
}

void cell::SimulationRunner::stopSimulation()
{
    if (thread_.joinable())
        thread_.request_stop();
}

void cell::SimulationRunner::setPerformanceDataCallback(void (*callback)(PerformanceData))
{
    performanceDataCallback_ = callback;
}

void cell::SimulationRunner::setSimulationStepDataCallback(void (*callback)(SimulationStepData))
{
    simulationStepDataCallback_ = callback;
}

void cell::SimulationRunner::loop(std::stop_token stopToken)
{
    auto start = ch::steady_clock::now();
    auto lastUpdate = start;
    auto timeSinceLastUpdate = ch::duration<double>(0s);
    auto simulationUpdateTime = ch::duration<double>(0s);
    const auto simulationTimeStep = ch::duration<double>(simulationConfig_.simulationTimeStep);
    int updates = 0;

    while (!stopToken.stop_requested())
    {
        auto now = ch::steady_clock::now();
        timeSinceLastUpdate += now - lastUpdate;
        lastUpdate = now;

        sendPerformanceData(start, updates, simulationUpdateTime);

        while (timeSinceLastUpdate / simulationConfig_.simulationTimeScale > simulationTimeStep &&
               !stopToken.stop_requested())
        {
            timeSinceLastUpdate -= simulationTimeStep / simulationConfig_.simulationTimeScale;

            const auto updateStart = ch::steady_clock::now();
            simulationFactory_.getCell().update(simulationTimeStep.count());
            simulationUpdateTime += ch::steady_clock::now() - updateStart;
            ++updates;

            sendPerformanceData(start, updates, simulationUpdateTime);
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
    if (!simulationFactory_.cellIsBuilt())
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
