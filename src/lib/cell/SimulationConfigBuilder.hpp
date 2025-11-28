#ifndef D95EF9F4_79A0_4AC2_8A12_B2EB5D8F4202_HPP
#define D95EF9F4_79A0_4AC2_8A12_B2EB5D8F4202_HPP

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

    // These are separate because if useDistribution is false, they won't be used anyways
    void setDiscCount(std::string membraneTypeName, int count);
    void setDistribution(std::string membraneTypeName, const std::unordered_map<std::string, double>& distribution);

    void addDiscType(const std::string& name, Radius radius, Mass mass);
    void addMembraneType(const std::string& name, Radius radius,
                         const std::unordered_map<std::string, MembraneType::Permeability>& permeabilityMap);
    void addMembrane(const std::string& membraneTypeName, Position position);
    void addReaction(const std::string& educt1, const std::string& educt2, const std::string& product1,
                     const std::string& product2, Probability probability);
    void setCellMembraneType(Radius radius,
                             const std::unordered_map<std::string, MembraneType::Permeability>& permeabilityMap);
    void setTimeStep(double simulationTimeStep);
    void setTimeScale(double simulationTimeScale);
    void setMaxVelocity(double maxVelocity);

    const SimulationConfig& getSimulationConfig() const;

private:
    SimulationConfig simulationConfig_;
};

} // namespace cell

#endif /* D95EF9F4_79A0_4AC2_8A12_B2EB5D8F4202_HPP */
