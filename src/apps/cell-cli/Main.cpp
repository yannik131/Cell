#include "cell/SimulationContext.hpp"
#include "cell/SimulationRecordSerializer.hpp"
#include "cell/SimulationRecorder.hpp"
#include "cell/SimulationRunner.hpp"

#include <CLI/CLI.hpp>

#include <chrono>
#include <filesystem>

namespace fs = std::filesystem;
using namespace std::chrono_literals;

int main(int argc, char** argv)
{
    CLI::App app{"Command line interface for the cell simulation"};

    fs::path configFile;
    fs::path outFile;
    double duration;

    CLI::Validator positiveDouble{[](const std::string& value) -> std::string
                                  {
                                      try
                                      {
                                          std::size_t pos = 0;
                                          const double result = std::stod(value, &pos);
                                          if (pos != value.size())
                                              throw std::exception{};
                                          else if (result <= 0.0)
                                              return "must be > 0";
                                          return {};
                                      }
                                      catch (...)
                                      {
                                          return "not a valid floating-point number";
                                      }
                                  },
                                  "POSITIVE_DOUBLE"};

    app.add_option("--config", configFile, "Config file")->required()->check(CLI::ExistingFile);
    app.add_option("--out", outFile, "Output file (type counts)")->required();
    app.add_option("--duration", duration, "Target simulation time in seconds")->required()->check(positiveDouble);

    CLI11_PARSE(app, argc, argv);

    cell::SimulationRunner simulationRunner;
    simulationRunner.useConfigFile(configFile);
    simulationRunner.setSimulationDuration(std::chrono::duration<double>{duration});

    cell::SimulationRecorder simulationRecorder(simulationRunner.getSimulationContext().discTypeRegistry,
                                                simulationRunner.getSimulationConfig());
    simulationRecorder.setStorageInterval(100ms);
    simulationRunner.setPerformanceDataCallback([&](auto data)
                                                { simulationRecorder.receivePerformanceData(std::move(data)); });
    simulationRunner.setPostUpdateCallback([&](cell::Cell& cell, const ch::duration<double>& elapsedTime)
                                           { simulationRecorder.processSimulationData(cell, elapsedTime); });

    simulationRunner.runSimulation();
    simulationRunner.waitForSimulationToFinish();
    simulationRecorder.storeRemainingData();

    cell::SimulationRecordSerializer simulationRecordSerializer;
    simulationRecordSerializer.writeTypeCountsToCsv(simulationRecorder.getDataPoints(),
                                                    simulationRunner.getSimulationContext().discTypeRegistry, outFile);

    return 0;
}