#ifndef AA6CB42A_4819_48FC_BAB5_42005AB904E5_HPP
#define AA6CB42A_4819_48FC_BAB5_42005AB904E5_HPP

#include "SimulationConfig.hpp"
#include "SimulationContext.hpp"

namespace cell
{

class Cell;
struct SimulationConfig;

class CellPopulator
{
public:
    CellPopulator(Cell& cell, SimulationConfig simulationConfig, SimulationContext simulationContext);

    void populateCell();

private:
    void populateWithDistributions();
    void populateDirectly();
    double calculateDistributionSum(const std::map<std::string, double>& distribution) const;

private:
    Cell& cell_;
    SimulationConfig simulationConfig_;
    SimulationContext simulationContext_;
};

} // namespace cell

#endif /* AA6CB42A_4819_48FC_BAB5_42005AB904E5_HPP */
