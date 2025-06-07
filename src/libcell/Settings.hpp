#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include "DiscType.hpp"
#include "Reaction.hpp"
#include "ReactionTable.hpp"

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
    sf::Time simulationTimeStep_ = sf::microseconds(5000);

    /**
     * @brief Defines how many seconds should pass in real time for 1 second in the simulation.
     *
     * Example: If set to 2, we will advance the simulation by 2 seconds in 1 real time second, meaning we will (try to)
     * call the update() method of the world 2 * 1000/simulationTimeStep_ times per second
     */
    float simulationTimeScale_ = 1.f;

    /**
     * @brief Total number of discs in the simulation
     */
    int numberOfDiscs_ = 100;

    /**
     * @brief Contains all disc types used for the simulation and their corresponding probabilities in percent
     */
    DiscType::map<int> discTypeDistribution_;

    /**
     * @brief Contains all reactions in the simulation
     */
    ReactionTable reactionTable_;
};

namespace SettingsLimits
{
const sf::Time MinSimulationTimeStep = sf::microseconds(100);
const sf::Time MaxSimulationTimeStep = sf::microseconds(50000);

const float MinSimulationTimeScale = 0.01f;
const float MaxSimulationTimeScale = 10.f;

const int MinNumberOfDiscs = 1;
const int MaxNumberOfDiscs = 10000;
} // namespace SettingsLimits

namespace DiscTypeLimits
{
const float MinRadius = 1.f;
const float MaxRadius = 100.f;

const float MinMass = 1.f;
const float MaxMass = 10000.f;
} // namespace DiscTypeLimits

#endif /* SETTINGS_HPP */
