#ifndef D9D3896F_DF9E_49A3_869C_E40E7D63EF6F_HPP
#define D9D3896F_DF9E_49A3_869C_E40E7D63EF6F_HPP

#include "cell/SimulationConfig.hpp"

#include <QStringList>

using PermeabilityMap = cell::config::PermeabilityMap;
// For editing we need a pair where .first is non-const, so PermeabilityMapEntry won't work
using PermeabilityMapEntry = std::pair<std::string, cell::MembraneType::Permeability>;
using DiscTypeDistribution = cell::config::DiscTypeDistribution;
using DiscTypeDistributionEntry = std::pair<std::string, double>;

struct RedrawOnly
{
    bool value;
};

struct DoReplot
{
    bool value;
};

struct SimulationRunning
{
    bool value;
};

struct Column
{
    int value;
};

#endif /* D9D3896F_DF9E_49A3_869C_E40E7D63EF6F_HPP */
