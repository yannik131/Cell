#ifndef D9D3896F_DF9E_49A3_869C_E40E7D63EF6F_HPP
#define D9D3896F_DF9E_49A3_869C_E40E7D63EF6F_HPP

#include "cell/SimulationConfig.hpp"
#include "cell/SimulationRecorder.hpp"

#include <QStringList>
#include <boost/histogram.hpp>

using PermeabilityMap = cell::config::PermeabilityMap;
// For editing we need a pair where .first is non-const, so PermeabilityMapEntry won't work
using PermeabilityMapEntry = std::pair<std::string, cell::MembraneType::Permeability>;
using DiscTypeDistribution = cell::config::DiscTypeDistribution;
using DiscTypeDistributionEntry = std::pair<std::string, double>;
using Frame = cell::SimulationRecorder::Frame;

namespace bh = boost::histogram;
using Histogram = bh::histogram<std::tuple<bh::axis::category<std::string>, bh::axis::regular<>>, bh::default_storage>;

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

struct Zoom
{
    double value;
};

struct CalculateSum
{
    bool value;
};

#endif /* D9D3896F_DF9E_49A3_869C_E40E7D63EF6F_HPP */
