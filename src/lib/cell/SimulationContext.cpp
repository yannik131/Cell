#include "SimulationContext.hpp"
#include "Disc.hpp"
#include "Settings.hpp"

#include <glog/logging.h>

#include <random>

namespace cell
{

SimulationContext::SimulationContext()
{
}

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

    auto discTypeResolver = discTypeRegistry_->getDiscTypeResolver();

    try
    {
        reactionTable_ =
            std::make_unique<ReactionTable>(buildReactionTable(simulationConfig, *discTypeRegistry_, discTypeResolver));
    }
    catch (const std::exception& e)
    {
        throw InvalidReactionsException(e.what());
    }

    try
    {
        auto simulationTimeStepProvider =
            buildSimulationTimeStepProvider(sf::seconds(static_cast<float>(simulationConfig.setup.simulationTimeStep)));
        auto maxRadiusProvider = [this]() -> double { return discTypeRegistry_->getMaxRadius(); };

        reactionEngine_ = std::make_unique<ReactionEngine>(
            buildReactionEngine(discTypeResolver, simulationTimeStepProvider, *reactionTable_));
        collisionDetector_ =
            std::make_unique<CollisionDetector>(buildCollisionDetector(discTypeResolver, maxRadiusProvider));
        collisionHandler_ = std::make_unique<CollisionHandler>(buildCollisionHandler(discTypeResolver));

        cell_ = std::make_unique<Cell>(
            buildCell(simulationConfig, maxRadiusProvider, simulationTimeStepProvider, discTypeResolver));
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

SimulationTimeStepProvider SimulationContext::buildSimulationTimeStepProvider(const sf::Time& simulationTimeStep)
{
    throwIfNotInRange(simulationTimeStep, SettingsLimits::MinSimulationTimeStep, SettingsLimits::MaxSimulationTimeStep,
                      "simulation time step");

    return std::function<double()>([simulationTimeStep]()
                                   { return static_cast<double>(simulationTimeStep.asSeconds()); });
}

DiscTypeRegistry SimulationContext::buildDiscTypeRegistry(const SimulationConfig& simulationConfig) const
{
    DiscTypeRegistry discTypeRegistry;
    std::vector<DiscType> discTypes;
    for (const auto& discType : simulationConfig.discTypes)
        discTypes.emplace_back(discType.name, Radius{discType.radius}, Mass{discType.mass});

    discTypeRegistry.setDiscTypes(std::move(discTypes));

    return discTypeRegistry;
}

ReactionTable SimulationContext::buildReactionTable(const SimulationConfig& simulationConfig,
                                                    const DiscTypeRegistry& discTypeRegistry,
                                                    DiscTypeResolver discTypeResolver) const
{
    ReactionTable reactionTable(discTypeResolver);

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

ReactionEngine SimulationContext::buildReactionEngine(DiscTypeResolver discTypeResolver,
                                                      SimulationTimeStepProvider simulationTimeStepProvider,
                                                      const ReactionTable& reactionTable) const
{
    return ReactionEngine(discTypeResolver, simulationTimeStepProvider, reactionTable);
}

CollisionDetector SimulationContext::buildCollisionDetector(DiscTypeResolver discTypeResolver,
                                                            MaxRadiusProvider maxRadiusProvider) const
{
    return CollisionDetector(discTypeResolver, maxRadiusProvider);
}

CollisionHandler SimulationContext::buildCollisionHandler(DiscTypeResolver discTypeResolver) const
{
    return CollisionHandler(discTypeResolver);
}

Cell SimulationContext::buildCell(const SimulationConfig& simulationConfig, MaxRadiusProvider maxRadiusProvider,
                                  SimulationTimeStepProvider simulationTimeStepProvider,
                                  DiscTypeResolver discTypeResolver) const
{
    std::vector<Disc> discs = getDiscsFromConfig(simulationConfig, maxRadiusProvider);

    Cell cell(*reactionEngine_, *collisionDetector_, *collisionHandler_, simulationTimeStepProvider, discTypeResolver,
              Dimensions{simulationConfig.setup.cellWidth, simulationConfig.setup.cellHeight}, std::move(discs));

    return cell;
}
std::vector<Disc> SimulationContext::getDiscsFromConfig(const SimulationConfig& simulationConfig,
                                                        MaxRadiusProvider maxRadiusProvider) const
{
    if (simulationConfig.setup.useDistribution)
        return createDiscGridFromDistribution(simulationConfig, maxRadiusProvider);

    return createDiscsDirectly(simulationConfig);
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
                                                                    MaxRadiusProvider maxRadiusProvider) const
{
    if (simulationConfig.setup.distribution.empty())
        throw ExceptionWithLocation("Can't distribute discs: Empty distribution");

    if (double total = calculateDistributionSum(simulationConfig.setup.distribution); std::abs(total - 1) > 1e-3)
    {
        throw ExceptionWithLocation("Percentages for disc type distribution don't add up to 100%. They add up to " +
                                    std::to_string(total));
    }

    std::vector<Disc> discs;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> velocityDistribution(-600.0, 600.0);

    discs.reserve(simulationConfig.setup.discCount);

    std::vector<sf::Vector2d> discPositions = mathutils::calculateGrid(
        simulationConfig.setup.cellWidth, simulationConfig.setup.cellHeight, maxRadiusProvider());

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
        int randomNumber = mathutils::getRandomNumber<double>(0, 1);

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