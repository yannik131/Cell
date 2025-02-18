#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include "DiscType.hpp"

#include <SFML/System/Time.hpp>

#include <map>
#include <utility>
#include <vector>

struct Settings
{
    Settings();

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
    int guiFPS_ = 30;

    /**
     * @brief How long to wait until resetting the collision count of the world and transmitting it to the gui for
     * plotting
     */
    sf::Time collisionUpdateTime_ = sf::milliseconds(1000);

    /**
     * @brief Total number of discs at the start of the simulation
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
     * @brief Contains reactions of type A + B -> C and their probabilities
     *
     * Example: If A + B -> C with 30% chance and A + B -> D with 20% chance, then
     * table[{A, B}] = table[{B, A}] = {{D, 0.2}, {C, 0.5}}
     * Accumulative probabilities sorted in ascending order are easier to work with with a random number approach
     */
    std::map<std::pair<DiscType, DiscType>, std::vector<std::pair<DiscType, float>>> combinationReactionTable_;

    /**
     * @brief Contains reactions of type C -> A + B and their probabilities to occur within 1 second
     */
    std::map<std::vector<std::pair<DiscType, float>>, std::pair<DiscType, DiscType>> decompositionReactionTable_;
};

namespace SettingsLimits
{
const sf::Time MinSimulationTimeStep = sf::milliseconds(1);
const sf::Time MaxSimulationTimeStep = sf::milliseconds(100);

const float MinSimulationTimeScale = 0.01f;
const float MaxSimulationTimeScale = 10.f;

const int MinGuiFPS = 0;
const int MaxGuiFPS = 200;

const sf::Time MinCollisionUpdateTime = sf::milliseconds(100);
const sf::Time MaxCollisionUpdateTime = sf::milliseconds(10000);

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
