#include "Settings.hpp"

namespace cell
{

namespace SettingsLimits
{
const std::chrono::microseconds MinSimulationTimeStep = std::chrono::microseconds{1};
const std::chrono::microseconds MaxSimulationTimeStep = std::chrono::milliseconds{100};

const double MinSimulationTimeScale = 0.0001f;
const double MaxSimulationTimeScale = 10.0;

const int MinNumberOfDiscs = 1;
const int MaxNumberOfDiscs = 10000;

const double MinCellWidth = 100;
const double MaxCellWidth = 100000;

const double MinCellHeight = 100;
const double MaxCellHeight = 100000;

const double MinMostProbableSpeed = 0;
const double MaxMostProbableSpeed = 100000;
} // namespace SettingsLimits

namespace DiscTypeLimits
{
const double MinRadius = 1.0;
const double MaxRadius = 100.0;

const double MinMass = 1.0;
const double MaxMass = 10000.0;
} // namespace DiscTypeLimits

namespace MembraneTypeLimits
{
const double MinRadius = 1.0;
const double MaxRadius = 100000.0;
} // namespace MembraneTypeLimits

} // namespace cell
