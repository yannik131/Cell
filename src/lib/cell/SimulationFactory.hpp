#ifndef EAA46EC4_DDB5_4CF5_A61A_8BC66872C559_HPP
#define EAA46EC4_DDB5_4CF5_A61A_8BC66872C559_HPP

#include "SimulationConfig.hpp"

namespace cell
{

class InvalidTypesException : public std::runtime_error
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
class Cell;
class Compartment;
class ReactionTable;
class ReactionEngine;
class CollisionDetector;
class CollisionHandler;
class Membrane;

class SimulationFactory
{
public:
    SimulationFactory();
    ~SimulationFactory();

    void buildSimulationFromConfig(const SimulationConfig& simulationConfig);
    SimulationContext getSimulationContext();

    Cell& getCell();
    bool cellIsBuilt() const;

    DiscTypeMap<int> getAndResetCollisionCounts();

private:
    ReactionTable buildReactionTable(const SimulationConfig& simulationConfig,
                                     const DiscTypeRegistry& discTypeRegistry);
    DiscTypeRegistry buildDiscTypeRegistry(const SimulationConfig& simulationConfig);
    MembraneTypeRegistry buildMembraneTypeRegistry(const SimulationConfig& simulationConfig);
    std::unique_ptr<Cell> buildCell(const SimulationConfig& simulationConfig);
    std::vector<Membrane> getMembranesFromConfig(const SimulationConfig& simulationConfig);
    void reset();
    void createCompartments(Cell& cell, std::vector<Membrane> membranes);
    void throwIfCompartmentsIntersect(const std::vector<Compartment*>& compartments) const;
    void throwIfDiscsCanBeLargerThanMembranes(const SimulationConfig& config) const;

private:
    std::unique_ptr<DiscTypeRegistry> discTypeRegistry_;
    std::unique_ptr<MembraneTypeRegistry> membraneTypeRegistry_;
    std::unique_ptr<ReactionTable> reactionTable_;
    std::unique_ptr<ReactionEngine> reactionEngine_;
    std::unique_ptr<CollisionHandler> collisionHandler_;
    std::unique_ptr<Cell> cell_;
};

} // namespace cell

#endif /* EAA46EC4_DDB5_4CF5_A61A_8BC66872C559_HPP */
