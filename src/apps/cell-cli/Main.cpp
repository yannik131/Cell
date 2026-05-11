#include "cell/SimulationRunner.hpp"

#include <CLI/CLI.hpp>

#include <filesystem>
#include <iostream>
#include <chrono>

namespace fs = std::filesystem;

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
    simulationRunner.setOutFile(outFile);
    simulationRunner.setSimulationTime(std::chrono::duration<double>{duration});
    
    simulationRunner.runSimulation();
    
    return 0;
}