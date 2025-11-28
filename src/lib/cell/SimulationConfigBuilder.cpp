#include "SimulationConfigBuilder.hpp"
#include "ExceptionWithLocation.hpp"

namespace cell
{

void SimulationConfigBuilder::addDisc(const std::string& discType, Position position, Velocity velocity)
{
    simulationConfig_.discs.push_back(
        config::Disc{.discTypeName = discType, .x = position.x, .y = position.y, .vx = velocity.x, .vy = velocity.y});
}

void SimulationConfigBuilder::useDistribution(bool useDistribution)
{
    simulationConfig_.useDistribution = useDistribution;
}

void SimulationConfigBuilder::setDiscCount(std::string membraneTypeName, int count)
{
    auto& membraneType = findMembraneTypeByName(simulationConfig_, membraneTypeName);
    membraneType.discCount = count;
}

void SimulationConfigBuilder::setDistribution(std::string membraneTypeName,
                                              const std::unordered_map<std::string, double>& distribution)
{
    auto& membraneType = findMembraneTypeByName(simulationConfig_, membraneTypeName);
    membraneType.discTypeDistribution = distribution;
}

void SimulationConfigBuilder::addDiscType(const std::string& name, Radius radius, Mass mass)
{
    simulationConfig_.discTypes.push_back(config::DiscType{.name = name, .radius = radius.value, .mass = mass.value});
}

void SimulationConfigBuilder::addMembraneType(
    const std::string& name, Radius radius,
    const std::unordered_map<std::string, MembraneType::Permeability>& permeabilityMap)
{
    simulationConfig_.membraneTypes.push_back(
        config::MembraneType{.name = name, .radius = radius.value, .permeabilityMap = permeabilityMap});
}

void SimulationConfigBuilder::addMembrane(const std::string& membraneTypeName, Position position)
{
    simulationConfig_.membranes.push_back(
        config::Membrane{.membraneTypeName = membraneTypeName, .x = position.x, .y = position.y});
}

void SimulationConfigBuilder::addReaction(const std::string& educt1, const std::string& educt2,
                                          const std::string& product1, const std::string& product2,
                                          Probability probability)
{
    simulationConfig_.reactions.push_back(config::Reaction{.educt1 = educt1,
                                                           .educt2 = educt2,
                                                           .product1 = product1,
                                                           .product2 = product2,
                                                           .probability = probability.value});
}

void SimulationConfigBuilder::setCellMembraneType(
    Radius radius, const std::unordered_map<std::string, MembraneType::Permeability>& permeabilityMap)
{
    simulationConfig_.cellMembraneType.radius = radius.value;
    simulationConfig_.cellMembraneType.permeabilityMap = permeabilityMap;
}

void SimulationConfigBuilder::setTimeStep(double simulationTimeStep)
{
    simulationConfig_.simulationTimeStep = simulationTimeStep;
}

void SimulationConfigBuilder::setTimeScale(double simulationTimeScale)
{
    simulationConfig_.simulationTimeScale = simulationTimeScale;
}

void SimulationConfigBuilder::setMaxVelocity(double maxVelocity)
{
    simulationConfig_.maxVelocity = maxVelocity;
}

const SimulationConfig& SimulationConfigBuilder::getSimulationConfig() const
{
    return simulationConfig_;
}

} // namespace cell