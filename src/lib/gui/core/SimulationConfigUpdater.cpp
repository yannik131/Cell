#include "core/SimulationConfigUpdater.hpp"
#include "SimulationConfigUpdater.hpp"

void SimulationConfigUpdater::createDiscTypeChangeMap(const std::vector<cell::config::DiscType>& newDiscTypes,
                                                      const std::vector<cell::config::DiscType>& oldDiscTypes,
                                                      const std::unordered_set<std::string>& removedDiscTypes)
{
    discTypeChangeMap_ = createChangeMap(newDiscTypes, oldDiscTypes, removedDiscTypes);
}

void SimulationConfigUpdater::createMembraneTypeChangeMap(
    const std::vector<cell::config::MembraneType>& newMembraneTypes,
    const std::vector<cell::config::MembraneType>& oldMembraneTypes,
    const std::unordered_set<std::string>& removedMembraneTypes)
{
    membraneTypeChangeMap_ = createChangeMap(newMembraneTypes, oldMembraneTypes, removedMembraneTypes);
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

    for (auto& [membraneTypeName, distribution] : config.setup.distributions)
    {
        for (auto iter = distribution.begin(); iter != distribution.end();)
        {
            if (removedDiscTypes.contains(iter->first))
                iter = distribution.erase(iter);
            else
                ++iter;
        }
    }

    config.setup.discs.erase(std::remove_if(config.setup.discs.begin(), config.setup.discs.end(),
                                            [&](const cell::config::Disc& disc)
                                            { return removedDiscTypes.contains(disc.discTypeName); }),
                             config.setup.discs.end());
}

void SimulationConfigUpdater::removeMembraneTypes(cell::SimulationConfig& config,
                                                  const std::unordered_set<std::string>& removedMembraneTypes) const
{
    config.setup.membranes.erase(std::remove_if(config.setup.membranes.begin(), config.setup.membranes.end(),
                                                [&](const cell::config::Disc& disc)
                                                { return removedMembraneTypes.contains(disc.discTypeName); }),
                                 config.setup.membranes.end());
}

void SimulationConfigUpdater::updateDiscTypes(cell::SimulationConfig& config) const
{
    if (discTypeChangeMap_.empty())
        return;

    for (auto& reaction : config.reactions)
    {
        reaction.educt1 = discTypeChangeMap_.at(reaction.educt1);
        reaction.educt2 = discTypeChangeMap_.at(reaction.educt2);
        reaction.product1 = discTypeChangeMap_.at(reaction.product1);
        reaction.product2 = discTypeChangeMap_.at(reaction.product2);
    }

    for (auto& [membraneTypeName, distribution] : config.setup.distributions)
    {
        std::map<std::string, double> newDistribution;

        for (const auto& [discType, frequency] : distribution)
            newDistribution[discTypeChangeMap_.at(discType)] = frequency;

        distribution = std::move(newDistribution);
    }

    for (auto& disc : config.setup.discs)
        disc.discTypeName = discTypeChangeMap_.at(disc.discTypeName);
}

void SimulationConfigUpdater::updateMembraneTypes(cell::SimulationConfig& config) const
{
    for (auto& membrane : config.setup.membranes)
        membrane.membraneTypeName = membraneTypeChangeMap_.at(membrane.membraneTypeName);
}

const auto& SimulationConfigUpdater::getDiscTypeChangeMap() const
{
    return discTypeChangeMap_;
}

const auto& SimulationConfigUpdater::getMembraneTypeChangeMap() const
{
    return membraneTypeChangeMap_;
}
