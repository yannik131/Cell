#ifndef SETTINGIDS_HPP
#define SETTINGIDS_HPP

#include "DiscType.hpp"
#include "ReactionTable.hpp"
#include "Settings.hpp"

namespace cell::SettingIDs
{

inline const SettingID<sf::Time> SimulationTimeStep("simulationTimeStep");
inline const SettingID<float> SimulationTimeScale("simulationTimeScale");
inline const SettingID<int> NumberOfDiscs("numberOfDiscs");
inline const SettingID<DiscType::map<int>> DiscTypeDistribution("discTypeDistribution");
inline const SettingID<ReactionTable> ReactionTable("reactionTable");

} // namespace cell::SettingIDs

#endif /* SETTINGIDS_HPP */
