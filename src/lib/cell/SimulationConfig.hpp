#ifndef CB5591CC_6EF7_4F94_AEED_D2110A4FB7CE_HPP
#define CB5591CC_6EF7_4F94_AEED_D2110A4FB7CE_HPP

#include "MembraneType.hpp"
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

struct MembraneType
{
    std::string name;
    double radius = 0;
    std::unordered_map<std::string, cell::MembraneType::Permeability> permeabilityMap;
    bool operator==(const MembraneType&) const = default;
};

struct Membrane
{
    std::string membraneTypeName;
    double x = 0, y = 0;
    bool operator==(const Membrane&) const = default;
};

struct Reaction
{
    std::string educt1, educt2, product1, product2;
    double probability = 0;
    bool operator==(const Reaction&) const = default;
};

inline const std::string cellMembraneTypeName = "Cell membrane";

struct Setup
{
    MembraneType cellMembraneType{.name = cellMembraneTypeName, .radius = 1000, .permeabilityMap = {}};

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
    std::map<std::string, int> discCounts;
    std::map<std::string, std::map<std::string, double>> distributions;

    // In case not:
    std::vector<config::Disc> discs;

    // These never use a distribution
    std::vector<config::Membrane> membranes;

    bool operator==(const Setup&) const = default;
};

} // namespace config

struct SimulationConfig
{
    std::vector<config::DiscType> discTypes;
    std::vector<config::MembraneType> membraneTypes;
    std::vector<config::Reaction> reactions;
    config::Setup setup;
    bool operator==(const SimulationConfig&) const = default;
};

namespace config
{

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DiscType, name, radius, mass)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Disc, discTypeName, x, y, vx, vy)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MembraneType, name, radius, permeabilityMap)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Membrane, membraneTypeName, x, y)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Reaction, educt1, educt2, product1, product2, probability)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Setup, simulationTimeStep, simulationTimeScale, maxVelocity, useDistribution,
                                   discCounts, distributions, discs)

} // namespace config

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SimulationConfig, discTypes, reactions, setup)

} // namespace cell

#endif /* CB5591CC_6EF7_4F94_AEED_D2110A4FB7CE_HPP */
