#ifndef AA6CB42A_4819_48FC_BAB5_42005AB904E5_HPP
#define AA6CB42A_4819_48FC_BAB5_42005AB904E5_HPP

#include "SimulationConfig.hpp"
#include "SimulationContext.hpp"

namespace cell
{

class Cell;
class Compartment;
struct SimulationConfig;
class Disc;

class CellPopulator
{
public:
    CellPopulator(Cell& cell, SimulationConfig simulationConfig, SimulationContext simulationContext);

    void populateCell();

private:
    void populateWithDistributions();
    void populateDirectly();
    double calculateDistributionSum(const std::map<std::string, double>& distribution) const;
    std::vector<sf::Vector2d> calculateCompartmentGridPoints(Compartment& compartment, double maxRadius,
                                                             int discCount) const;
    void populateCompartmentWithDistribution(Compartment& compartment, double maxRadius);
    sf::Vector2d sampleVelocityFromDistribution() const;
    Compartment& findDeepestContainingCompartment(const Disc& disc);
    double calculateValueSum(const std::map<std::string, double>& distribution) const;
    std::vector<std::pair<DiscTypeID, double>>
    calculateAccumulatedPercentages(const std::map<std::string, double>& distribution,
                                    const std::string& membraneTypeName) const;

private:
    Cell& cell_;
    SimulationConfig simulationConfig_;
    SimulationContext simulationContext_;
};

} // namespace cell

#endif /* AA6CB42A_4819_48FC_BAB5_42005AB904E5_HPP */
