#ifndef SIMULATIONCONFIG_HPP
#define SIMULATIONCONFIG_HPP

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
    double cellWidth;
    double cellHeight;
    double simulationTimeStep;
    double simulationTimeScale = 1;
    bool useDistribution = false;

    // In case of distribution:
    int discCount;
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
