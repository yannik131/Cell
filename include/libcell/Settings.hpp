#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include "DiscType.hpp"
#include "Reaction.hpp"

#include <SFML/System/Time.hpp>

#include <map>
#include <utility>
#include <vector>

struct Settings
{
    /**
     * @brief Time that passes between single simulation steps. Smaller value means more accurate collisions, but
     * requires more updates to advance the simulation in time. If this value is too small, the simulation might not be
     * able to keep up and start lagging
     */
    sf::Time simulationTimeStep_ = sf::milliseconds(5);

    /**
     * @brief Defines how many seconds should pass in real time for 1 second in the simulation.
     *
     * Example: If set to 2, we will advance the simulation by 2 seconds in 1 real time second, meaning we will (try to)
     * call the update() method of the world 2 * 1000/simulationTimeStep_ times per second
     */
    float simulationTimeScale_ = 1.f;

    /**
     * @brief How many times per second the simulation sends frame data to the gui for updating the visual
     * representation of the simulation
     * @note Frame data transmission takes about 10us for a few hundred discs, so 30 FPS will use about 0.3ms each
     * second (time during which the simulation can't update)
     */
    int guiFPS_ = 60;

    /**
     * @brief How long to wait between plots
     */
    sf::Time plotTimeInterval_ = sf::milliseconds(1000);

    /**
     * @brief Total number of discs in the simulation
     */
    int numberOfDiscs_ = 50;

    /**
     * @brief Global friction coefficient for all disc collisions
     */
    float frictionCoefficient = 0.f;

    /**
     * @brief Contains all disc types used for the simulation and their corresponding probabilities in percent
     */
    std::map<DiscType, int> discTypeDistribution_;

    /**
     * @brief Maps disc types to decomposition reactions A -> B + C by educt
     * @todo Lookup for decomposition reactions takes up some time though, maybe unordered_map is worth consideration
     */
    std::map<DiscType, std::vector<Reaction>> decompositionReactions_;

    /**
     * @brief Maps pairs of disc types to combination reactions A + B -> C by educts. {A, B} and {B, A} map to the same
     * reactions
     */
    std::map<std::pair<DiscType, DiscType>, std::vector<Reaction>> combinationReactions_;

    /**
     * @brief Maps pairs of disc types to exchange reactions A + B -> C + D by educts. {A, B} and {B, A} map to the same
     * reactions
     */
    std::map<std::pair<DiscType, DiscType>, std::vector<Reaction>> exchangeReactions_;
};

namespace SettingsLimits
{
const sf::Time MinSimulationTimeStep = sf::milliseconds(1);
const sf::Time MaxSimulationTimeStep = sf::milliseconds(100);

const float MinSimulationTimeScale = 0.01f;
const float MaxSimulationTimeScale = 10.f;

const int MinGuiFPS = 0;
const int MaxGuiFPS = 200;

const sf::Time MinPlotTimeInterval = sf::milliseconds(100);
const sf::Time MaxPlotTimeInterval = sf::milliseconds(10000);

const int MinNumberOfDiscs = 1;
const int MaxNumberOfDiscs = 10000;

const float MinFrictionCoefficient = 0.f;
const float MaxFrictionCoefficient = 1.f;
} // namespace SettingsLimits

namespace DiscTypeLimits
{
const float MinRadius = 1.f;
const float MaxRadius = 100.f;

const float MinMass = 1.f;
const float MaxMass = 10000.f;
} // namespace DiscTypeLimits

#endif /* SETTINGS_HPP */
