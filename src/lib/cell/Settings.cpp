#include "Settings.hpp"

namespace cell
{

namespace SettingsLimits
{
const sf::Time MinSimulationTimeStep = sf::microseconds(1);
const sf::Time MaxSimulationTimeStep = sf::milliseconds(100);

const double MinSimulationTimeScale = 0.0001f;
const double MaxSimulationTimeScale = 10.0;

const int MinNumberOfDiscs = 1;
const int MaxNumberOfDiscs = 10000;

const double MinCellWidth = 100;
const double MaxCellWidth = 100000;

const double MinCellHeight = 100;
const double MaxCellHeight = 100000;

const double MinMaxVelocity = 0;
const double MaxMaxVelocity = 1000;
} // namespace SettingsLimits

namespace DiscTypeLimits
{
const double MinRadius = 1.0;
const double MaxRadius = 100.0;

const double MinMass = 1.0;
const double MaxMass = 10000.0;
} // namespace DiscTypeLimits

} // namespace cell
