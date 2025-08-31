#include "SimulationContext.hpp"
#include "Disc.hpp"
#include "Settings.hpp"

#include <glog/logging.h>

#include <random>

namespace cell
{

SimulationContext::SimulationContext()
    : simulationTimeStepProvider_([&]() -> double { return simulationTimeStep_.asSeconds(); })
    , maxRadiusProvider_([&]() -> double { return discTypeRegistry_->getMaxRadius(); })
{
}

void SimulationContext::buildContextFromConfig(const SimulationConfig& simulationConfig)
{
    setSimulationTimeStep(sf::seconds(static_cast<float>(simulationConfig.simulationTimeStep)));
    setSimulationTimeScale(simulationConfig.simulationTimeScale);
    discTypeRegistry_ = std::make_unique<DiscTypeRegistry>(buildDiscTypeRegistry(simulationConfig));
    discTypeResolver_ = discTypeRegistry_->getDiscTypeResolver();
    reactionTable_ = std::make_unique<ReactionTable>(buildReactionTable(simulationConfig));
    reactionEngine_ = std::make_unique<ReactionEngine>(buildReactionEngine());
    collisionDetector_ = std::make_unique<CollisionDetector>(buildCollisionDetector());
    collisionHandler_ = std::make_unique<CollisionHandler>(buildCollisionHandler());
    cell_ = std::make_unique<Cell>(buildCell(simulationConfig));
}

const DiscTypeRegistry& SimulationContext::getDiscTypeRegistry() const
{
    return *discTypeRegistry_;
}

Cell& SimulationContext::getCell()
{
    return *cell_;
}

DiscTypeMap<int> SimulationContext::getAndResetCollisionCounts()
{
    return collisionDetector_->getAndResetCollisionCounts();
}

void SimulationContext::setSimulationTimeStep(const sf::Time& simulationTimeStep)
{
    throwIfNotInRange(simulationTimeStep, SettingsLimits::MinSimulationTimeStep, SettingsLimits::MaxSimulationTimeStep,
                      "simulation time step");

    simulationTimeStep_ = simulationTimeStep;
}

void SimulationContext::setSimulationTimeScale(double simulationTimeScale)
{
    throwIfNotInRange(simulationTimeScale, SettingsLimits::MinSimulationTimeScale,
                      SettingsLimits::MaxSimulationTimeScale, "simulation time scale");

    simulationTimeScale_ = simulationTimeScale;
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

ReactionTable SimulationContext::buildReactionTable(const SimulationConfig& simulationConfig) const
{
    if (!discTypeRegistry_)
        throw ExceptionWithLocation("Need a valid disc type registry");

    ReactionTable reactionTable(discTypeResolver_);

    for (const auto& reaction : simulationConfig.reactions)
    {
        DiscTypeID educt1 = discTypeRegistry_->getIDFor(reaction.educt1);
        std::optional<DiscTypeID> educt2 =
            reaction.educt2.empty() ? std::nullopt : std::make_optional(discTypeRegistry_->getIDFor(reaction.educt2));

        DiscTypeID product1 = discTypeRegistry_->getIDFor(reaction.product1);
        std::optional<DiscTypeID> product2 = reaction.product2.empty()
                                                 ? std::nullopt
                                                 : std::make_optional(discTypeRegistry_->getIDFor(reaction.product2));

        Reaction newReaction(educt1, educt2, product1, product2, reaction.probability);
        reactionTable.addReaction(newReaction);
    }

    return reactionTable;
}

ReactionEngine SimulationContext::buildReactionEngine() const
{
    return ReactionEngine(discTypeResolver_, simulationTimeStepProvider_, *reactionTable_);
}
CollisionDetector SimulationContext::buildCollisionDetector() const
{
    return CollisionDetector(discTypeResolver_, maxRadiusProvider_);
}
CollisionHandler SimulationContext::buildCollisionHandler() const
{
    return CollisionHandler(discTypeResolver_);
}
Cell SimulationContext::buildCell(const SimulationConfig& simulationConfig) const
{
    std::vector<Disc> discs = getDiscsFromConfig(simulationConfig);

    Cell cell(*reactionEngine_, *collisionDetector_, *collisionHandler_, simulationTimeStepProvider_, discTypeResolver_,
              Dimensions{simulationConfig.cellWidth, simulationConfig.cellHeight}, std::move(discs));

    return cell;
}
std::vector<Disc> SimulationContext::getDiscsFromConfig(const SimulationConfig& simulationConfig) const
{
    // always prefer discs if provided
    if (!simulationConfig.discs.empty())
        return createDiscsDirectly(simulationConfig);

    return createDiscGridFromDistribution(simulationConfig);
}
std::vector<Disc> SimulationContext::createDiscsDirectly(const SimulationConfig& simulationConfig) const
{
    std::vector<Disc> discs;

    for (const auto& disc : simulationConfig.discs)
    {
        Disc newDisc(discTypeRegistry_->getIDFor(disc.discTypeName));
        newDisc.setPosition({disc.x, disc.y});
        newDisc.setVelocity({disc.vx, disc.vy});

        discs.push_back(std::move(newDisc));
    }

    return discs;
}
std::vector<Disc> SimulationContext::createDiscGridFromDistribution(const SimulationConfig& simulationConfig) const
{
    if (simulationConfig.distribution.empty() || simulationConfig.discCount == 0)
        throw ExceptionWithLocation("Must provider either discs or disc type distribution with disc count");

    if (double total = calculateDistributionSum(simulationConfig.distribution); std::abs(total - 100) > 1e-3)
    {
        throw ExceptionWithLocation("Percentages for disc type distribution don't add up to 100. They add up to " +
                                    std::to_string(total));
    }

    std::vector<Disc> discs;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(0, 100);
    std::uniform_real_distribution<double> velocityDistribution(-600.0, 600.0);

    discs.reserve(simulationConfig.discCount);

    std::vector<sf::Vector2d> discPositions =
        mathutils::calculateGrid(simulationConfig.cellWidth, simulationConfig.cellHeight, maxRadiusProvider_());

    if (simulationConfig.discCount > static_cast<int>(discPositions.size()))
    {
        LOG(WARNING) << "According to the settings, " << std::to_string(simulationConfig.discCount)
                     << " discs should be created, but the grid can only fit " << std::to_string(discPositions.size())
                     << ". " << std::to_string(simulationConfig.discCount - discPositions.size())
                     << " discs will not be created.";
    }

    // We need the accumulated percentages sorted in ascending order for the random number approach to work
    std::vector<std::pair<DiscTypeID, double>> discTypes;
    for (const auto& pair : simulationConfig.distribution)
    {
        DiscTypeID ID = discTypeRegistry_->getIDFor(pair.first);
        discTypes.emplace_back(ID, pair.second + (discTypes.empty() ? 0 : discTypes.back().second));
    }

    std::ranges::sort(discTypes, [](const auto& a, const auto& b) { return a.second < b.second; });

    for (int i = 0; i < simulationConfig.discCount && !discPositions.empty(); ++i)
    {
        int randomNumber = distribution(gen);

        for (const auto& [discType, percentage] : discTypes)
        {
            if (randomNumber < percentage || percentage == 100)
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
    return std::accumulate(distribution.begin(), distribution.end(), 0,
                           [](double currentSum, auto& entryPair) { return currentSum + entryPair.second; });
}

} // namespace cell