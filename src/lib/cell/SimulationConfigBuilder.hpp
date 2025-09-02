#ifndef SIMULATIONCONFIGBUILDER_HPP
#define SIMULATIONCONFIGBUILDER_HPP

#include "SimulationConfig.hpp"
#include "Types.hpp"

namespace cell
{

/**
 * @brief Convenience class for integration testing to easily build a simulation state
 */
class SimulationConfigBuilder
{
public:
    void addDisc(const std::string& discType, Position position, Velocity velocity);
    void useDistribution(bool useDistribution);
    void setDiscCount(int count);
    void setDistribution(const std::map<std::string, double>& distribution);
    void addDiscType(const std::string& name, Radius radius, Mass mass);
    void addReaction(const std::string& educt1, const std::string& educt2, const std::string& product1,
                     const std::string& product2, Probability probability);
    void setCellDimensions(Width width, Height height);
    void setTimeStep(double simulationTimeStep);
    void setTimeScale(double simulationTimeScale);

    const SimulationConfig& getSimulationConfig() const;

private:
    int requiredCallsCount_ = 0;
    SimulationConfig simulationConfig_;
};

} // namespace cell

#endif /* SIMULATIONCONFIGBUILDER_HPP */
