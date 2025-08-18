#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include "DiscType.hpp"
#include "Reaction.hpp"
#include "ReactionTable.hpp"
#include "SFMLJsonSerializers.hpp"

#include <SFML/System/Time.hpp>
#include <nlohmann/json.hpp>

#include <map>
#include <utility>
#include <vector>

namespace cell
{

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
    double simulationTimeScale_ = 1.0;

    /**
     * @brief Total number of discs in the simulation
     */
    int numberOfDiscs_ = 100;

    /**
     * @brief Width of the cell, arbitrary unit
     */
    int cellWidth_ = 1000;

    /**
     * @brief Height of the cell, arbitrary unit
     */
    int cellHeight_ = 1000;

    /**
     * @brief Stores the percentage of each disc type for the start of the simulation
     */
    DiscType::map<int> discTypeDistribution_;

    /**
     * @brief The central storage for all disc types in the simulation
     *
     */
    std::vector<DiscType> discTypes_;

    /**
     * @brief Contains all reactions in the simulation
     */
    ReactionTable reactionTable_;
};

/* NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Settings, simulationTimeStep_, simulationTimeScale_, numberOfDiscs_, cellWidth_,
                                   cellHeight_, discTypeDistribution_, discTypes_, reactionTable_) */

namespace SettingsLimits
{
const sf::Time MinSimulationTimeStep = sf::microseconds(1);
const sf::Time MaxSimulationTimeStep = sf::microseconds(100000);

const double MinSimulationTimeScale = 0.0001f;
const double MaxSimulationTimeScale = 10.0;

const int MinNumberOfDiscs = 1;
const int MaxNumberOfDiscs = 10000;

const int MinCellWidth = 100;
const int MaxCellWidth = 100000;

const int MinCellHeight = 100;
const int MaxCellHeight = 100000;
} // namespace SettingsLimits

namespace DiscTypeLimits
{
const double MinRadius = 1.0;
const double MaxRadius = 100.0;

const double MinMass = 1.0;
const double MaxMass = 10000.0;
} // namespace DiscTypeLimits

} // namespace cell

#endif /* SETTINGS_HPP */