#include "cell/SimulationContext.hpp"
#include "cell/SimulationRecordSerializer.hpp"
#include "cell/SimulationRecorder.hpp"
#include "cell/SimulationRunner.hpp"
#include "cell/StringUtils.hpp"

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
    double duration{};
    double storageInterval{};

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
    app.add_option("--storage-interval", storageInterval, "Storage interval in seconds")
        ->required()
        ->check(positiveDouble);

    CLI11_PARSE(app, argc, argv);

    cell::SimulationRunner simulationRunner;
    simulationRunner.useConfigFile(configFile);
    simulationRunner.setSimulationDuration(std::chrono::duration<double>{duration});

    cell::SimulationRecorder simulationRecorder(simulationRunner.getSimulationContext().discTypeRegistry,
                                                simulationRunner.getSimulationConfig().mostProbableSpeed);
    simulationRecorder.setStorageInterval(ch::duration<double>(storageInterval));
    simulationRunner.setPerformanceDataCallback([&](auto data)
                                                { simulationRecorder.printPerformanceData(std::move(data)); });
    simulationRunner.setPostBuildCallback([&](cell::Cell& cell)
                                          { simulationRecorder.processInitialSimulationData(cell); });
    simulationRunner.setPostUpdateCallback([&](cell::Cell& cell, const ch::duration<double>& elapsedTime)
                                           { simulationRecorder.processSimulationData(cell, elapsedTime); });

    std::cout << "Starting simulation\n";
    const auto start = ch::steady_clock::now();
    simulationRunner.runSimulation();
    simulationRunner.waitForSimulationToFinish();
    const auto elapsed = ch::steady_clock::now() - start;
    std::cout << "Finished simulation in " << cell::stringutils::timeString(elapsed.count()) << "\n";

    simulationRecorder.storeRemainingData();

    cell::SimulationRecordSerializer simulationRecordSerializer;
    simulationRecordSerializer.writeTypeCountsToCsv(simulationRecorder.getDataPoints(),
                                                    simulationRunner.getSimulationContext().discTypeRegistry, outFile);

    return 0;
}