#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include "DiscType.hpp"

#include <QtCore/QMetaType>
#include <SFML/System/Time.hpp>

#include <map>

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
     * Example: If set to 2, we will advance the simulation by 2 seconds in 1 real time second, meaning we will call the
     * update() method of the world 2 * 1000/simulationTimeStep_ times per second
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
     * @brief Contains all disc types used for the simulation and their corresponding probabilities in percent
     */
    std::map<DiscType, int> discTypeDistribution_;

    const sf::Time MinSimulationTimeStep = sf::milliseconds(1);
    const sf::Time MaxSimulationTimeStep = sf::milliseconds(100);

    const float MinSimulationTimeScale = 0.1f;
    const float MaxSimulationTimeScale = 10.f;

    const int MinGuiFPS = 1;
    const int MaxGuiFPS = 60;

    const sf::Time MinCollisionUpdateTime = sf::milliseconds(100);
    const sf::Time MaxCollisionUpdateTime = sf::milliseconds(10000);

    const int MinNumberOfDiscs = 1;
    const int MaxNumberOfDiscs = 10000;

    // Limits for DiscType

    struct
    {
        const float MinRadius = 1.f;
        const float MaxRadius = 100.f;

        const float MinMass = 1.f;
        const float MaxMass = 100.f;
    } discTypeLimits;
};

Q_DECLARE_METATYPE(Settings);

#endif /* SETTINGS_HPP */
