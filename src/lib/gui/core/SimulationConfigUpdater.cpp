#include "core/SimulationConfigUpdater.hpp"
#include "SimulationConfigUpdater.hpp"
#include "cell/SimulationFactory.hpp"
#include "core/SFMLJsonSerializers.hpp"

#include <nlohmann/json.hpp>

#include <fstream>

using json = nlohmann::json;

const cell::SimulationConfig& SimulationConfigUpdater::getSimulationConfig() const
{
    return simulationConfig_;
}

void SimulationConfigUpdater::setSimulationConfig(const cell::SimulationConfig& simulationConfig)
{
    const bool discTypesDidChange = simulationConfig_.discTypes != simulationConfig.discTypes;

    testConfig(simulationConfig);
    simulationConfig_ = simulationConfig;

    if (discTypesDidChange)
        emit discTypesChanged();

    emit configChanged();
}

const std::map<std::string, sf::Color>& SimulationConfigUpdater::getDiscTypeColorMap() const
{
    return discTypeColorMap_;
}

const std::map<std::string, sf::Color>& SimulationConfigUpdater::getMembraneTypeColorMap() const
{
    return membraneTypeColorMap_;
}

void SimulationConfigUpdater::saveConfigToFile(const fs::path& path) const
{
    json j;
    j["config"] = simulationConfig_;
    j["discTypeColorMap"] = discTypeColorMap_;
    j["membraneTypeColorMap"] = membraneTypeColorMap_;

    std::ofstream file(path);
    file << j.dump(4);
}

void SimulationConfigUpdater::loadConfigFromFile(const fs::path& path)
{
    json j;
    std::ifstream file(path);
    file >> j;

    setSimulationConfig(j["config"].get<cell::SimulationConfig>());
    discTypeColorMap_ = j["discTypeColorMap"].get<std::map<std::string, sf::Color>>();
    membraneTypeColorMap_ = j["membraneTypeColorMap"].get<std::map<std::string, sf::Color>>();
}

void SimulationConfigUpdater::removeDiscTypes(cell::SimulationConfig& config,
                                              const std::unordered_set<std::string>& removedDiscTypes) const
{
    if (removedDiscTypes.empty())
        return;

    config.reactions.erase(std::remove_if(config.reactions.begin(), config.reactions.end(),
                                          [&](const cell::config::Reaction& reaction)
                                          {
                                              return removedDiscTypes.contains(reaction.educt1) ||
                                                     removedDiscTypes.contains(reaction.educt2) ||
                                                     removedDiscTypes.contains(reaction.product1) ||
                                                     removedDiscTypes.contains(reaction.product2);
                                          }),
                           config.reactions.end());

    for (auto& membraneType : config.membraneTypes)
    {
        auto& distribution = membraneType.discTypeDistribution;
        for (auto iter = distribution.begin(); iter != distribution.end();)
        {
            if (removedDiscTypes.contains(iter->first))
                iter = distribution.erase(iter);
            else
                ++iter;
        }
    }

    config.discs.erase(std::remove_if(config.discs.begin(), config.discs.end(), [&](const cell::config::Disc& disc)
                                      { return removedDiscTypes.contains(disc.discTypeName); }),
                       config.discs.end());
}

void SimulationConfigUpdater::removeMembraneTypes(cell::SimulationConfig& config,
                                                  const std::unordered_set<std::string>& removedMembraneTypes) const
{
    config.membranes.erase(std::remove_if(config.membranes.begin(), config.membranes.end(),
                                          [&](const cell::config::Membrane& membrane)
                                          { return removedMembraneTypes.contains(membrane.membraneTypeName); }),
                           config.membranes.end());
}

void SimulationConfigUpdater::testConfig(const cell::SimulationConfig& simulationConfig) const
{
    cell::SimulationFactory simulationFactory;
    simulationFactory.buildSimulationFromConfig(simulationConfig);
}

void SimulationConfigUpdater::updateDiscTypes(cell::SimulationConfig& config,
                                              const std::unordered_map<std::string, std::string>& changeMap) const
{
    if (changeMap.empty())
        return;

    for (auto& reaction : config.reactions)
    {
        reaction.educt1 = changeMap.at(reaction.educt1);
        reaction.educt2 = changeMap.at(reaction.educt2);
        reaction.product1 = changeMap.at(reaction.product1);
        reaction.product2 = changeMap.at(reaction.product2);
    }

    for (auto& membraneType : config.membraneTypes)
    {
        cell::config::DiscTypeDistribution newDistribution;

        for (const auto& [discType, frequency] : membraneType.discTypeDistribution)
            newDistribution[changeMap.at(discType)] = frequency;

        membraneType.discTypeDistribution = std::move(newDistribution);
    }

    for (auto& disc : config.discs)
    {
        if (!changeMap.contains(disc.discTypeName))
            continue;

        disc.discTypeName = changeMap.at(disc.discTypeName);
    }
}

void SimulationConfigUpdater::updateMembraneTypes(cell::SimulationConfig& config,
                                                  const std::unordered_map<std::string, std::string>& changeMap) const
{
    for (auto& membrane : config.membranes)
    {
        if (!changeMap.contains(membrane.membraneTypeName))
            continue;

        membrane.membraneTypeName = changeMap.at(membrane.membraneTypeName);
    }
}
