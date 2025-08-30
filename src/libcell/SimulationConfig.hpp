#ifndef SIMULATIONCONFIG_HPP
#define SIMULATIONCONFIG_HPP

#include "Vector2d.hpp"

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

} // namespace config

struct SimulationConfig
{
    std::vector<config::DiscType> discTypes;
    std::vector<config::Disc> discs;
    std::vector<config::Reaction> reactions;
    int cellWidth;
    int cellHeight;
    double simulationTimeStep;
    double simulationTimeScale = 1;
};

} // namespace cell

#endif /* SIMULATIONCONFIG_HPP */
