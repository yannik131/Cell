#include "core/SimulationConfigUpdater.hpp"
#include "SimulationConfigUpdater.hpp"

void SimulationConfigUpdater::removeDiscTypes(cell::SimulationConfig& config,
                                              const std::unordered_set<std::string>& removedDiscTypes)
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

    for (auto iter = config.setup.distribution.begin(); iter != config.setup.distribution.end();)
    {
        if (removedDiscTypes.contains(iter->first))
            iter = config.setup.distribution.erase(iter);
        else
            ++iter;
    }

    config.setup.discs.erase(std::remove_if(config.setup.discs.begin(), config.setup.discs.end(),
                                            [&](const cell::config::Disc& disc)
                                            { return removedDiscTypes.contains(disc.discTypeName); }),
                             config.setup.discs.end());
}

std::map<std::string, std::string>
SimulationConfigUpdater::createChangeMap(const std::vector<cell::config::DiscType>& newDiscTypes,
                                         const std::vector<cell::config::DiscType>& oldDiscTypes,
                                         const std::unordered_set<std::string>& removedDiscTypes)
{
    // Since new disc types are always appended to the table, iterating both arrays in order gives the changes
    // We'll map "" to "" to accomodate empty strings like in reactions

    std::map<std::string, std::string> changeMap({{"", ""}});
    for (std::size_t i = 0; i < oldDiscTypes.size() && i < newDiscTypes.size(); ++i)
    {
        if (removedDiscTypes.contains(oldDiscTypes[i].name))
            continue;

        changeMap[oldDiscTypes[i].name] = newDiscTypes[i].name;
    }

    return changeMap;
}

void SimulationConfigUpdater::updateDiscTypes(cell::SimulationConfig& config,
                                              const std::map<std::string, std::string>& changeMap)
{
    for (auto& reaction : config.reactions)
    {
        reaction.educt1 = changeMap.at(reaction.educt1);
        reaction.educt2 = changeMap.at(reaction.educt2);
        reaction.product1 = changeMap.at(reaction.product1);
        reaction.product2 = changeMap.at(reaction.product2);
    }

    std::map<std::string, double> newDistribution;
    for (const auto& [discType, frequency] : config.setup.distribution)
        newDistribution[changeMap.at(discType)] = frequency;
    config.setup.distribution = std::move(newDistribution);

    for (auto& disc : config.setup.discs)
        disc.discTypeName = changeMap.at(disc.discTypeName);
}
