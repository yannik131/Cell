#ifndef B3854111_59D8_4367_AEF9_0248DAF108BC_HPP
#define B3854111_59D8_4367_AEF9_0248DAF108BC_HPP

#include "cell/SimulationConfig.hpp"

#include <unordered_set>

class SimulationConfigUpdater
{
public:
    void createDiscTypeChangeMap(const std::vector<cell::config::DiscType>& newDiscTypes,
                                 const std::vector<cell::config::DiscType>& oldDiscTypes,
                                 const std::unordered_set<std::string>& removedDiscTypes);
    void createMembraneTypeChangeMap(const std::vector<cell::config::MembraneType>& newMembraneTypes,
                                     const std::vector<cell::config::MembraneType>& oldMembraneTypes,
                                     const std::unordered_set<std::string>& removedMembraneTypes);

    void removeDiscTypes(cell::SimulationConfig& config, const std::unordered_set<std::string>& removedDiscTypes) const;
    void removeMembraneTypes(cell::SimulationConfig& config,
                             const std::unordered_set<std::string>& removedMembraneTypes) const;

    void updateDiscTypes(cell::SimulationConfig& config) const;
    void updateMembraneTypes(cell::SimulationConfig& config) const;

    template <typename T>
    std::unordered_map<std::string, std::string>
    createChangeMap(const std::vector<T>& newTypes, const std::vector<T>& oldTypes,
                    const std::unordered_set<std::string>& removedTypes) const;

private:
    std::unordered_map<std::string, std::string> discTypeChangeMap_;
    std::unordered_map<std::string, std::string> membraneTypeChangeMap_;
};

template <typename T>
inline std::unordered_map<std::string, std::string>
SimulationConfigUpdater::createChangeMap(const std::vector<T>& newTypes, const std::vector<T>& oldTypes,
                                         const std::unordered_set<std::string>& removedTypes) const
{
    // Since new types are always appended to the table in the GUI, iterating both arrays in order gives the changes
    // We'll map "" to "" to accomodate empty strings like in reactions

    std::map<std::string, std::string> changeMap({{"", ""}});
    for (std::size_t i = 0; i < oldTypes.size() && i < newTypes.size(); ++i)
    {
        if (removedTypes.contains(oldTypes[i].name))
            continue;

        changeMap[oldTypes[i].name] = newTypes[i].name;
    }

    if (changeMap.size() == 1)
        return {};

    return changeMap;
}

#endif /* B3854111_59D8_4367_AEF9_0248DAF108BC_HPP */
