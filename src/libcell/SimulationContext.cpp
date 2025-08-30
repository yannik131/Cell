#include "SimulationContext.hpp"
#include "Settings.hpp"

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
        newReaction.validate(discTypeResolver_);

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
    CellState cellState(discTypeResolver_, maxRadiusProvider_);
    cellState.setCellHeight(simulationConfig.cellHeight);
    cellState.setCellWidth(simulationConfig.cellWidth);

    std::vector<Disc> discs;
    for (const auto& disc : simulationConfig.discs)
    {
        Disc newDisc(discTypeRegistry_->getIDFor(disc.discTypeName));
        newDisc.setPosition({disc.x, disc.y});
        newDisc.setVelocity({disc.vx, disc.vy});

        discs.push_back(std::move(newDisc));
    }

    cellState.setDiscs(std::move(discs));

    Cell cell(*reactionEngine_, *collisionDetector_, *collisionHandler_, simulationTimeStepProvider_);
    cell.setState(std::move(cellState));

    return cell;
}
} // namespace cell