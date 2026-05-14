#include "cell/Logging.hpp"
#include "cell/SimulationContext.hpp"
#include "cell/SimulationRecordSerializer.hpp"
#include "cell/SimulationRecorder.hpp"
#include "cell/SimulationRunner.hpp"

#include <gflags/gflags.h>

#include <chrono>
#include <filesystem>

namespace fs = std::filesystem;
using namespace std::chrono_literals;

DEFINE_string(config, "", "Config file");
DEFINE_string(out, "", "Output file (type counts)");
DEFINE_double(duration, 0.0, "Target simulation time in seconds");

int main(int argc, char** argv)
{
    gflags::SetUsageMessage("cell-cli --config <file> --out <file> --duration <seconds>\n"
                            "Runs the cell simulation.");
    gflags::SetVersionString("cell-cli 1.0");
    cell::initLogging(argc, argv);

    gflags::ParseCommandLineFlags(&argc, &argv, true);

    if (FLAGS_config.empty())
    {
        LOG(INFO) << "--config is required";
        return 0;
    }

    if (!fs::exists(FLAGS_config))
    {
        LOG(INFO) << "--config must be an existing file";
        return 0;
    }

    if (FLAGS_out.empty())
    {
        LOG(INFO) << "--out is required";
        return 0;
    }

    if (FLAGS_duration <= 0.0)
    {
        LOG(INFO) << "--duration must be > 0";
        return 0;
    }

    fs::path configFile;
    fs::path outFile;
    double duration;

    cell::SimulationRunner simulationRunner;
    simulationRunner.useConfigFile(configFile);
    simulationRunner.setSimulationDuration(std::chrono::duration<double>{duration});

    cell::SimulationRecorder simulationRecorder;
    simulationRecorder.setStorageInterval(100ms);
    simulationRunner.setPerformanceDataCallback([&](auto data)
                                                { simulationRecorder.receivePerformanceData(std::move(data)); });
    simulationRunner.setPostUpdateCallback(
        [&](cell::Cell& cell, const cell::SimulationContext& simulationContext,
            const std::chrono::duration<double>& elapsedTime)
        { simulationRecorder.processSimulationData(cell, simulationContext, elapsedTime); });

    simulationRunner.runSimulation();
    simulationRunner.waitForSimulationToFinish();
    simulationRecorder.storeRemainingData();

    cell::SimulationRecordSerializer simulationRecordSerializer;
    simulationRecordSerializer.writeTypeCountsToCsv(simulationRecorder.getDataPoints(),
                                                    simulationRunner.getSimulationContext().discTypeRegistry, outFile);

    return 0;
}