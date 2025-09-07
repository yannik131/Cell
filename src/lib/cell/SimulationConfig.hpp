#ifndef SIMULATIONCONFIG_HPP
#define SIMULATIONCONFIG_HPP

#include "Settings.hpp"
#include "Vector2d.hpp"

#include <map>
#include <string>
#include <vector>

namespace cell
{

namespace config
{

struct DiscType
{
    std::string name;
    double radius;
    double mass;
};

struct Disc
{
    std::string discTypeName;
    double x, y;
    double vx, vy;
};

struct Reaction
{
    std::string educt1, educt2, product1, product2;
    double probability;
};

struct Setup
{
    double cellWidth = SettingsLimits::MinCellWidth;
    double cellHeight = SettingsLimits::MinCellHeight;

    /**
     * @brief Time that passes between single simulation steps. Smaller value means more accurate collisions, but
     * requires more updates to advance the simulation in time. If this value is too small, the simulation might not be
     * able to keep up and start lagging
     */
    double simulationTimeStep = SettingsLimits::MinSimulationTimeStep.asSeconds();

    /**
     * @brief Defines how many seconds should pass in real time for 1 second in the simulation.
     *
     * Example: If set to 2, we will advance the simulation by 2 seconds in 1 real time second, meaning we will (try to)
     * call the update() method of the world 2 * 1000/simulationTimeStep_ times per second
     */
    double simulationTimeScale = 1;

    bool useDistribution = false;

    // In case of distribution:
    int discCount = SettingsLimits::MinNumberOfDiscs;
    std::map<std::string, double> distribution;

    // In case not:
    std::vector<config::Disc> discs;
};

} // namespace config

struct SimulationConfig
{
    std::vector<config::DiscType> discTypes;
    std::vector<config::Reaction> reactions;
    config::Setup setup;
};

} // namespace cell

#endif /* SIMULATIONCONFIG_HPP */
