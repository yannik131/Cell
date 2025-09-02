#ifndef SIMULATIONCONTEXT_HPP
#define SIMULATIONCONTEXT_HPP

#include "Cell.hpp"
#include "CollisionDetector.hpp"
#include "CollisionHandler.hpp"
#include "DiscTypeRegistry.hpp"
#include "ReactionEngine.hpp"
#include "ReactionTable.hpp"
#include "SimulationConfig.hpp"

#include <SFML/System/Time.hpp>

namespace cell
{

class SimulationContext
{
public:
    SimulationContext();

    void buildContextFromConfig(const SimulationConfig& simulationConfig);

    const DiscTypeRegistry& getDiscTypeRegistry() const;
    Cell& getCell();

    DiscTypeMap<int> getAndResetCollisionCounts();

private:
    void setSimulationTimeStep(const sf::Time& simulationTimeStep);
    void setSimulationTimeScale(double simulationTimeScale);
    DiscTypeRegistry buildDiscTypeRegistry(const SimulationConfig& simulationConfig) const;
    ReactionTable buildReactionTable(const SimulationConfig& simulationConfig) const;
    ReactionEngine buildReactionEngine() const;
    CollisionDetector buildCollisionDetector() const;
    CollisionHandler buildCollisionHandler() const;
    Cell buildCell(const SimulationConfig& simulationConfig) const;

    std::vector<Disc> getDiscsFromConfig(const SimulationConfig& simulationConfig) const;
    std::vector<Disc> createDiscsDirectly(const SimulationConfig& simulationConfig) const;
    std::vector<Disc> createDiscGridFromDistribution(const SimulationConfig& simulationConfig) const;

    double calculateDistributionSum(const std::map<std::string, double>& distribution) const;

private:
    /**
     * @brief Time that passes between single simulation steps. Smaller value means more accurate collisions, but
     * requires more updates to advance the simulation in time. If this value is too small, the simulation might not be
     * able to keep up and start lagging
     */
    sf::Time simulationTimeStep_ = sf::microseconds(5000);

    /**
     * @brief Defines how many seconds should pass in real time for 1 second in the simulation.
     *
     * Example: If set to 2, we will advance the simulation by 2 seconds in 1 real time second, meaning we will (try to)
     * call the update() method of the world 2 * 1000/simulationTimeStep_ times per second
     */
    double simulationTimeScale_ = 1.0;

    SimulationTimeStepProvider simulationTimeStepProvider_;
    MaxRadiusProvider maxRadiusProvider_;
    DiscTypeResolver discTypeResolver_;

    std::unique_ptr<DiscTypeRegistry> discTypeRegistry_;
    std::unique_ptr<ReactionTable> reactionTable_;
    std::unique_ptr<ReactionEngine> reactionEngine_;
    std::unique_ptr<CollisionDetector> collisionDetector_;
    std::unique_ptr<CollisionHandler> collisionHandler_;
    std::unique_ptr<Cell> cell_;
};

} // namespace cell

#endif /* SIMULATIONCONTEXT_HPP */