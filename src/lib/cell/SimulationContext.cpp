#include "SimulationContext.hpp"
#include "Disc.hpp"
#include "Membrane.hpp"
#include "Settings.hpp"

#include <glog/logging.h>

#include <random>

namespace cell
{

void SimulationContext::buildContextFromConfig(const SimulationConfig& simulationConfig)
{
    built_ = false;

    try
    {
        discTypeRegistry_ = std::make_unique<DiscTypeRegistry>(buildDiscTypeRegistry(simulationConfig));
    }
    catch (const std::exception& e)
    {
        throw InvalidDiscTypesException(e.what());
    }

    try
    {
        reactionTable_ =
            std::make_unique<ReactionTable>(buildReactionTable(simulationConfig, std::as_const(*discTypeRegistry_)));
    }
    catch (const std::exception& e)
    {
        throw InvalidReactionsException(e.what());
    }

    try
    {
        reactionEngine_ =
            std::make_unique<ReactionEngine>(std::as_const(*discTypeRegistry_), std::as_const(*reactionTable_));
        collisionDetector_ = std::make_unique<CollisionDetector>(std::as_const(*discTypeRegistry_));
        collisionHandler_ = std::make_unique<CollisionHandler>(std::as_const(*discTypeRegistry_));

        cell_ = std::make_unique<Cell>(buildCell(simulationConfig));
    }
    catch (const std::exception& e)
    {
        throw InvalidSetupException(e.what());
    }

    built_ = true;
}

const DiscTypeRegistry& SimulationContext::getDiscTypeRegistry() const
{
    throwIfNotBuildYet();

    return *discTypeRegistry_;
}

Cell& SimulationContext::getCell()
{
    throwIfNotBuildYet();

    return *cell_;
}

DiscTypeMap<int> SimulationContext::getAndResetCollisionCounts()
{
    throwIfNotBuildYet();

    return collisionDetector_->getAndResetCollisionCounts();
}

DiscTypeRegistry SimulationContext::buildDiscTypeRegistry(const SimulationConfig& simulationConfig) const
{
    DiscTypeRegistry discTypeRegistry;
    std::vector<DiscType> discTypes;
    for (const auto& discType : simulationConfig.discTypes)
        discTypes.emplace_back(discType.name, Radius{discType.radius}, Mass{discType.mass});

    discTypeRegistry.setValues(std::move(discTypes));

    return discTypeRegistry;
}

MembraneTypeRegistry SimulationContext::buildMembraneTypeRegistry(const SimulationConfig& simulationConfig) const
{
    MembraneTypeRegistry registry;
    std::vector<MembraneType> types;
    for (const auto& type : simulationConfig.membraneTypes)
    {
        MembraneType::PermeabilityMap permeabilityMap;
        for (const auto& [discTypeName, permeability] : type.permeabilityMap)
            permeabilityMap[discTypeRegistry_->getIDFor(discTypeName)] = permeability;

        types.emplace_back(type.name, type.radius, std::move(permeabilityMap));
    }

    registry.setValues(std::move(types));

    return registry;
}

ReactionTable SimulationContext::buildReactionTable(const SimulationConfig& simulationConfig,
                                                    const DiscTypeRegistry& discTypeRegistry) const
{
    ReactionTable reactionTable(discTypeRegistry);

    for (const auto& reaction : simulationConfig.reactions)
    {
        DiscTypeID educt1 = discTypeRegistry.getIDFor(reaction.educt1);
        std::optional<DiscTypeID> educt2 =
            reaction.educt2.empty() ? std::nullopt : std::make_optional(discTypeRegistry.getIDFor(reaction.educt2));

        DiscTypeID product1 = discTypeRegistry.getIDFor(reaction.product1);
        std::optional<DiscTypeID> product2 =
            reaction.product2.empty() ? std::nullopt : std::make_optional(discTypeRegistry.getIDFor(reaction.product2));

        Reaction newReaction(educt1, educt2, product1, product2, reaction.probability);
        reactionTable.addReaction(newReaction);
    }

    return reactionTable;
}

Cell SimulationContext::buildCell(const SimulationConfig& simulationConfig) const
{
    std::vector<Disc> discs = getDiscsFromConfig(simulationConfig);
    std::vector<Membrane> membranes = getMembranesFromConfig(simulationConfig);

    Cell cell(*reactionEngine_, *collisionDetector_, *collisionHandler_, std::as_const(*discTypeRegistry_),
              std::as_const(*membraneTypeRegistry_),
              Dimensions{.width = simulationConfig.setup.cellWidth, .height = simulationConfig.setup.cellHeight},
              std::move(discs), std::move(membranes));

    return cell;
}
std::vector<Disc> SimulationContext::getDiscsFromConfig(const SimulationConfig& simulationConfig) const
{
    if (simulationConfig.setup.useDistribution)
    {
        const auto& discTypes = simulationConfig.discTypes;
        if (discTypes.empty())
            return {};

        double maxRadius = std::max_element(discTypes.begin(), discTypes.end(),
                                            [](const config::DiscType& lhs, const config::DiscType& rhs)
                                            { return lhs.radius < rhs.radius; })
                               ->radius;

        return createDiscGridFromDistribution(simulationConfig, maxRadius);
    }

    return createDiscsDirectly(simulationConfig);
}

std::vector<Membrane> SimulationContext::getMembranesFromConfig(const SimulationConfig& simulationConfig) const
{
    std::vector<Membrane> membranes;

    for (const auto& membrane : simulationConfig.setup.membranes)
    {
        Membrane newMembrane(membraneTypeRegistry_->getIDFor(membrane.membraneTypeName));
        newMembrane.setPosition({membrane.x, membrane.y});

        membranes.push_back(std::move(newMembrane));
    }

    return membranes;
}

std::vector<Disc> SimulationContext::createDiscsDirectly(const SimulationConfig& simulationConfig) const
{
    std::vector<Disc> discs;

    for (const auto& disc : simulationConfig.setup.discs)
    {
        Disc newDisc(discTypeRegistry_->getIDFor(disc.discTypeName));
        newDisc.setPosition({disc.x, disc.y});
        newDisc.setVelocity({disc.vx, disc.vy});

        discs.push_back(std::move(newDisc));
    }

    return discs;
}

std::vector<Disc> SimulationContext::createDiscGridFromDistribution(const SimulationConfig& simulationConfig,
                                                                    double maxRadius) const
{
    if (simulationConfig.setup.distribution.empty())
        return {};

    if (double total = calculateDistributionSum(simulationConfig.setup.distribution); std::abs(total - 1) > 1e-3)
    {
        throw ExceptionWithLocation("Percentages for disc type distribution don't add up to 100%. They add up to " +
                                    std::to_string(total));
    }

    std::vector<Disc> discs;
    std::random_device rd;
    std::mt19937 gen(rd());

    throwIfNotInRange(simulationConfig.setup.maxVelocity, SettingsLimits::MinMaxVelocity,
                      SettingsLimits::MaxMaxVelocity, "Max. velocity");
    std::uniform_real_distribution<double> velocityDistribution(-simulationConfig.setup.maxVelocity,
                                                                simulationConfig.setup.maxVelocity);

    discs.reserve(simulationConfig.setup.discCount);

    std::vector<sf::Vector2d> discPositions =
        mathutils::calculateGrid(simulationConfig.setup.cellWidth, simulationConfig.setup.cellHeight, maxRadius);

    if (simulationConfig.setup.discCount > static_cast<int>(discPositions.size()))
    {
        LOG(WARNING) << "According to the settings, " << std::to_string(simulationConfig.setup.discCount)
                     << " discs should be created, but the grid can only fit " << std::to_string(discPositions.size())
                     << ". " << std::to_string(simulationConfig.setup.discCount - discPositions.size())
                     << " discs will not be created.";
    }

    // We need the accumulated percentages sorted in ascending order for the random number approach to work
    std::vector<std::pair<DiscTypeID, double>> discTypes;
    for (const auto& pair : simulationConfig.setup.distribution)
    {
        DiscTypeID ID = discTypeRegistry_->getIDFor(pair.first);
        discTypes.emplace_back(ID, pair.second + (discTypes.empty() ? 0 : discTypes.back().second));
    }

    std::ranges::sort(discTypes, [](const auto& a, const auto& b) { return a.second < b.second; });

    for (int i = 0; i < simulationConfig.setup.discCount && !discPositions.empty(); ++i)
    {
        auto randomNumber = mathutils::getRandomNumber<double>(0, 1);

        for (const auto& [discType, percentage] : discTypes)
        {
            if (randomNumber < percentage)
            {
                Disc newDisc(discType);
                newDisc.setPosition(discPositions.back());
                newDisc.setVelocity(sf::Vector2d(velocityDistribution(gen), velocityDistribution(gen)));

                discs.push_back(newDisc);
                discPositions.pop_back();

                break;
            }
        }
    }

    return discs;
}

double SimulationContext::calculateDistributionSum(const std::map<std::string, double>& distribution) const
{
    return std::accumulate(distribution.begin(), distribution.end(), 0.0,
                           [](double currentSum, auto& entryPair) { return currentSum + entryPair.second; });
}

void SimulationContext::throwIfNotBuildYet() const
{
    if (!built_)
        throw ExceptionWithLocation("Simulation context was not yet fully built");
}

bool SimulationContext::isBuilt() const
{
    return built_;
}

} // namespace cell