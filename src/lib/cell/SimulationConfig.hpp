#ifndef SIMULATIONCONFIG_HPP
#define SIMULATIONCONFIG_HPP

#include "Settings.hpp"
#include "Vector2d.hpp"

#include <map>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

namespace cell
{

namespace config
{

struct DiscType
{
    std::string name;
    double radius = 0;
    double mass = 0;
    bool operator==(const DiscType&) const = default;
};

struct Disc
{
    std::string discTypeName;
    double x = 0, y = 0;
    double vx = 0, vy = 0;
    bool operator==(const Disc&) const = default;
};

struct Reaction
{
    std::string educt1, educt2, product1, product2;
    double probability = 0;
    bool operator==(const Reaction&) const = default;
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
    double simulationTimeStep = sf::milliseconds(1).asSeconds();

    /**
     * @brief Defines how many seconds should pass in real time for 1 second in the simulation.
     *
     * Example: If set to 2, we will advance the simulation by 2 seconds in 1 real time second, meaning we will (try to)
     * call the update() method of the world 2 * 1000/simulationTimeStep_ times per second
     */
    double simulationTimeScale = 1;
    double maxVelocity = 600;

    bool useDistribution = true;

    // In case of distribution:
    int discCount = SettingsLimits::MinNumberOfDiscs;
    std::map<std::string, double> distribution;

    // In case not:
    std::vector<config::Disc> discs;

    bool operator==(const Setup&) const = default;
};

} // namespace config

struct SimulationConfig
{
    std::vector<config::DiscType> discTypes;
    std::vector<config::Reaction> reactions;
    config::Setup setup;
    bool operator==(const SimulationConfig&) const = default;
};

namespace config
{

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DiscType, name, radius, mass)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Disc, discTypeName, x, y, vx, vy)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Reaction, educt1, educt2, product1, product2, probability)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Setup, cellWidth, cellHeight, simulationTimeStep, simulationTimeScale, maxVelocity,
                                   useDistribution, discCount, distribution, discs)

} // namespace config

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SimulationConfig, discTypes, reactions, setup)

} // namespace cell

#endif /* SIMULATIONCONFIG_HPP */
