#ifndef SIMULATIONCONFIGUPDATER_HPP
#define SIMULATIONCONFIGUPDATER_HPP

#include "cell/SimulationConfig.hpp"

#include <unordered_set>

class SimulationConfigUpdater
{
public:
    void removeDiscTypes(cell::SimulationConfig& config, const std::unordered_set<std::string>& removedDiscTypes);
    std::map<std::string, std::string> createChangeMap(const std::vector<cell::config::DiscType>& newDiscTypes,
                                                       const std::vector<cell::config::DiscType>& oldDiscTypes,
                                                       const std::unordered_set<std::string>& removedDiscTypes);

    void updateDiscTypes(cell::SimulationConfig& config, const std::map<std::string, std::string>& changeMap);

    const std::map<std::string, std::string>& getChangeMap() const;
};

#endif /* SIMULATIONCONFIGUPDATER_HPP */
