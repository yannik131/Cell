#ifndef F1160089_C2A5_45FA_AC16_370C293275DE_HPP
#define F1160089_C2A5_45FA_AC16_370C293275DE_HPP

#include <filesystem>
#include <chrono>

namespace fs = std::filesystem;

namespace cell
{
    
class SimulationRunner
{
public:
    void useConfigFile(const fs::path& configFile);
    void setOutFile(const fs::path& outFile);
    void setSimulationTime(const std::chrono::duration<double>& simulationTime);
    void runSimulation();
};
    
}

#endif /* F1160089_C2A5_45FA_AC16_370C293275DE_HPP */
