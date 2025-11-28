#ifndef B3854111_59D8_4367_AEF9_0248DAF108BC_HPP
#define B3854111_59D8_4367_AEF9_0248DAF108BC_HPP

#include "cell/SimulationConfig.hpp"

#include <QObject>
#include <SFML/Graphics/Color.hpp>

#include <unordered_set>

class SimulationConfigUpdater : public QObject
{
    Q_OBJECT
public:
    const cell::SimulationConfig& getSimulationConfig() const;
    void setSimulationConfig(const cell::SimulationConfig& simulationConfig);

    template <typename T>
    void setTypes(const std::vector<T>& newTypes, const std::unordered_set<std::string>& removedTypes,
                  const std::map<std::string, sf::Color>& colorMap);

    const std::map<std::string, sf::Color>& getDiscTypeColorMap() const;
    const std::map<std::string, sf::Color>& getMembraneTypeColorMap() const;

    void setFPS(int FPS);
    int getFPS() const;

    void saveConfigToFile(const fs::path& path) const;
    void loadConfigFromFile(const fs::path& path);

signals:
    void discTypesChanged();

private:
    void removeDiscTypes(cell::SimulationConfig& config, const std::unordered_set<std::string>& removedDiscTypes) const;
    void removeMembraneTypes(cell::SimulationConfig& config,
                             const std::unordered_set<std::string>& removedMembraneTypes) const;

    void updateDiscTypes(cell::SimulationConfig& config,
                         const std::unordered_map<std::string, std::string>& changeMap) const;
    void updateMembraneTypes(cell::SimulationConfig& config,
                             const std::unordered_map<std::string, std::string>& changeMap) const;
    void testConfig(const cell::SimulationConfig& simulationConfig) const;

    template <typename T>
    std::unordered_map<std::string, std::string>
    createChangeMap(const std::vector<T>& newTypes, const std::vector<T>& oldTypes,
                    const std::unordered_set<std::string>& removedTypes) const;

private:
    cell::SimulationConfig simulationConfig_;
    std::map<std::string, sf::Color> discTypeColorMap_;
    std::map<std::string, sf::Color> membraneTypeColorMap_;
    int FPS_ = 60;
};

template <typename T>
inline void SimulationConfigUpdater::setTypes(const std::vector<T>& newTypes,
                                              const std::unordered_set<std::string>& removedTypes,
                                              const std::map<std::string, sf::Color>& colorMap)
{
    auto simulationConfigCopy = simulationConfig_;

    if constexpr (std::is_same_v<T, cell::config::DiscType>)
    {
        removeDiscTypes(simulationConfigCopy, removedTypes);
        auto changeMap = createChangeMap(newTypes, simulationConfig_.discTypes, removedTypes);
        updateDiscTypes(simulationConfigCopy, changeMap);
        simulationConfigCopy.discTypes = newTypes;
        discTypeColorMap_ = colorMap;
        emit discTypesChanged();
    }
    else if constexpr (std::is_same_v<T, cell::config::MembraneType>)
    {
        removeMembraneTypes(simulationConfigCopy, removedTypes);
        auto changeMap = createChangeMap(newTypes, simulationConfig_.membraneTypes, removedTypes);
        updateMembraneTypes(simulationConfigCopy, changeMap);
        simulationConfigCopy.membraneTypes = newTypes;
        membraneTypeColorMap_ = colorMap;
    }

    setSimulationConfig(simulationConfigCopy);
}

template <typename T>
inline std::unordered_map<std::string, std::string>
SimulationConfigUpdater::createChangeMap(const std::vector<T>& newTypes, const std::vector<T>& oldTypes,
                                         const std::unordered_set<std::string>& removedTypes) const
{
    static_assert(std::is_same_v<T, cell::config::DiscType> || std::is_same_v<T, cell::config::MembraneType>,
                  "Unsupported type");

    // Since new types are always appended to the table in the GUI, iterating both arrays in order gives the changes
    // We'll map "" to "" to accomodate empty strings like in reactions

    std::unordered_map<std::string, std::string> changeMap({{"", ""}});

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
