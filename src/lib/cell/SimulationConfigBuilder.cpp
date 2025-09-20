#include "SimulationConfigBuilder.hpp"
#include "ExceptionWithLocation.hpp"

namespace cell
{

void SimulationConfigBuilder::addDisc(const std::string& discType, Position position, Velocity velocity)
{
    simulationConfig_.setup.discs.push_back(
        config::Disc{.discTypeName = discType, .x = position.x, .y = position.y, .vx = velocity.x, .vy = velocity.y});
}

void SimulationConfigBuilder::useDistribution(bool useDistribution)
{
    simulationConfig_.setup.useDistribution = useDistribution;
}

void SimulationConfigBuilder::setDiscCount(int count)
{
    simulationConfig_.setup.discCount = count;
}

void SimulationConfigBuilder::setDistribution(const std::map<std::string, double>& distribution)
{
    simulationConfig_.setup.distribution = distribution;
}

void SimulationConfigBuilder::addDiscType(const std::string& name, Radius radius, Mass mass)
{
    simulationConfig_.discTypes.push_back(config::DiscType{.name = name, .radius = radius.value, .mass = mass.value});
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

void SimulationConfigBuilder::setCellDimensions(Width width, Height height)
{
    ++requiredCallsCount_;
    simulationConfig_.setup.cellWidth = width.value;
    simulationConfig_.setup.cellHeight = height.value;
}

void SimulationConfigBuilder::setTimeStep(double simulationTimeStep)
{
    ++requiredCallsCount_;
    simulationConfig_.setup.simulationTimeStep = simulationTimeStep;
}

void SimulationConfigBuilder::setTimeScale(double simulationTimeScale)
{
    simulationConfig_.setup.simulationTimeScale = simulationTimeScale;
}

void SimulationConfigBuilder::setMaxVelocity(double maxVelocity)
{
    simulationConfig_.setup.maxVelocity = maxVelocity;
}

const SimulationConfig& SimulationConfigBuilder::getSimulationConfig() const
{
    if (requiredCallsCount_ != 2)
        throw ExceptionWithLocation("Config has not been fully built yet");

    return simulationConfig_;
}

} // namespace cell