#ifndef EAA46EC4_DDB5_4CF5_A61A_8BC66872C559_HPP
#define EAA46EC4_DDB5_4CF5_A61A_8BC66872C559_HPP

#include "SimulationConfig.hpp"

#include <SFML/System/Time.hpp>

namespace cell
{

class InvalidDiscTypesException : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

class InvalidReactionsException : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

class InvalidSetupException : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

struct SimulationContext;

class SimulationFactory
{
public:
    SimulationFactory();

    void buildSimulationFromConfig(const SimulationConfig& simulationConfig);
    SimulationContext getSimulationContext() const;

    Cell& getCell();
    bool isBuilt() const;

    DiscTypeMap<int> getAndResetCollisionCounts();

private:
    ReactionTable buildReactionTable(const SimulationConfig& simulationConfig,
                                     const DiscTypeRegistry& discTypeRegistry) const;
    DiscTypeRegistry buildDiscTypeRegistry(const SimulationConfig& simulationConfig) const;
    MembraneTypeRegistry buildMembraneTypeRegistry(const SimulationConfig& simulationConfig) const;
    Cell buildCell(const SimulationConfig& simulationConfig) const;
    std::vector<Membrane> getMembranesFromConfig(const SimulationConfig& simulationConfig) const;

    double calculateDistributionSum(const std::map<std::string, double>& distribution) const;

    void throwIfNotBuildYet() const;

private:
    std::unique_ptr<DiscTypeRegistry> discTypeRegistry_;
    std::unique_ptr<MembraneTypeRegistry> membraneTypeRegistry_;
    std::unique_ptr<ReactionTable> reactionTable_;
    std::unique_ptr<ReactionEngine> reactionEngine_;
    std::unique_ptr<CollisionDetector> collisionDetector_;
    std::unique_ptr<CollisionHandler> collisionHandler_;
    std::unique_ptr<Cell> cell_;

    bool built_ = false;
};

} // namespace cell

#endif /* EAA46EC4_DDB5_4CF5_A61A_8BC66872C559_HPP */
