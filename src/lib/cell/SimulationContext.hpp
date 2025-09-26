#ifndef EAA46EC4_DDB5_4CF5_A61A_8BC66872C559_HPP
#define EAA46EC4_DDB5_4CF5_A61A_8BC66872C559_HPP

#include "Cell.hpp"
#include "CollisionDetector.hpp"
#include "CollisionHandler.hpp"
#include "ReactionEngine.hpp"
#include "ReactionTable.hpp"
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

class SimulationContext
{
public:
    SimulationContext() = default;

    void buildContextFromConfig(const SimulationConfig& simulationConfig);

    const DiscTypeRegistry& getDiscTypeRegistry() const;
    Cell& getCell();
    bool isBuilt() const;

    DiscTypeMap<int> getAndResetCollisionCounts();

private:
    DiscTypeRegistry buildDiscTypeRegistry(const SimulationConfig& simulationConfig) const;
    ReactionTable buildReactionTable(const SimulationConfig& simulationConfig,
                                     const DiscTypeRegistry& discTypeRegistry) const;
    ReactionEngine buildReactionEngine(const DiscTypeRegistry& discTypeRegistry,
                                       SimulationTimeStepProvider simulationTimeStepProvider,
                                       const ReactionTable& reactionTable) const;
    CollisionDetector buildCollisionDetector(const DiscTypeRegistry& discTypeRegistry,
                                             MaxRadiusProvider maxRadiusProvider) const;
    CollisionHandler buildCollisionHandler(const DiscTypeRegistry& discTypeRegistry) const;
    Cell buildCell(const SimulationConfig& simulationConfig, MaxRadiusProvider maxRadiusProvider,
                   SimulationTimeStepProvider simulationTimeStepProvider,
                   const DiscTypeRegistry& discTypeRegistry) const;

    std::vector<Disc> getDiscsFromConfig(const SimulationConfig& simulationConfig,
                                         MaxRadiusProvider maxRadiusProvider) const;
    std::vector<Disc> createDiscsDirectly(const SimulationConfig& simulationConfig) const;
    std::vector<Disc> createDiscGridFromDistribution(const SimulationConfig& simulationConfig,
                                                     MaxRadiusProvider maxRadiusProvider) const;

    double calculateDistributionSum(const std::map<std::string, double>& distribution) const;

    void throwIfNotBuildYet() const;

private:
    std::unique_ptr<DiscTypeRegistry> discTypeRegistry_;
    std::unique_ptr<ReactionTable> reactionTable_;
    std::unique_ptr<ReactionEngine> reactionEngine_;
    std::unique_ptr<CollisionDetector> collisionDetector_;
    std::unique_ptr<CollisionHandler> collisionHandler_;
    std::unique_ptr<Cell> cell_;

    bool built_ = false;
};

} // namespace cell

#endif /* EAA46EC4_DDB5_4CF5_A61A_8BC66872C559_HPP */
