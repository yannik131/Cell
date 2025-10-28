#include "SimulationConfig.hpp"

cell::config::MembraneType& cell::findMembraneTypeByName(cell::SimulationConfig& simulationConfig,
                                                         std::string membraneTypeName)
{
    if (membraneTypeName == "" || membraneTypeName == cell::config::cellMembraneTypeName)
        return simulationConfig.cellMembraneType;

    auto membraneType = std::find_if(simulationConfig.membraneTypes.begin(), simulationConfig.membraneTypes.end(),
                                     [&](const auto& membraneType) { return membraneType.name == membraneTypeName; });
    if (membraneType == simulationConfig.membraneTypes.end())
        throw ExceptionWithLocation("No membrane type \"" + membraneTypeName + "\" found in the config");

    return *membraneType;
}