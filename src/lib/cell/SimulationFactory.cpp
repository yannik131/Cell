#include "SimulationFactory.hpp"
#include "Cell.hpp"
#include "CollisionDetector.hpp"
#include "CollisionHandler.hpp"
#include "Disc.hpp"
#include "Membrane.hpp"
#include "ReactionEngine.hpp"
#include "ReactionTable.hpp"
#include "Settings.hpp"
#include "SimulationContext.hpp"

#include <glog/logging.h>

#include <random>

namespace cell
{
SimulationFactory::SimulationFactory() = default;

void SimulationFactory::buildSimulationFromConfig(const SimulationConfig& simulationConfig)
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

SimulationContext SimulationFactory::getSimulationContext() const
{
    throwIfNotBuildYet();

    return SimulationContext{.discTypeRegistry = *discTypeRegistry_,
                             .membraneTypeRegistry = *membraneTypeRegistry_,
                             .reactionEngine = *reactionEngine_,
                             .collisionDetector = *collisionDetector_,
                             .collisionHandler = *collisionHandler_};
}

Cell& SimulationFactory::getCell()
{
    throwIfNotBuildYet();

    return *cell_;
}

DiscTypeMap<int> SimulationFactory::getAndResetCollisionCounts()
{
    throwIfNotBuildYet();

    return collisionDetector_->getAndResetCollisionCounts();
}

DiscTypeRegistry SimulationFactory::buildDiscTypeRegistry(const SimulationConfig& simulationConfig) const
{
    DiscTypeRegistry discTypeRegistry;
    std::vector<DiscType> discTypes;
    for (const auto& discType : simulationConfig.discTypes)
        discTypes.emplace_back(discType.name, Radius{discType.radius}, Mass{discType.mass});

    discTypeRegistry.setValues(std::move(discTypes));

    return discTypeRegistry;
}

MembraneTypeRegistry SimulationFactory::buildMembraneTypeRegistry(const SimulationConfig& simulationConfig) const
{
    MembraneTypeRegistry registry;
    std::vector<MembraneType> types;

    auto convertPermeabilityMap = [&](const std::unordered_map<std::string, MembraneType::Permeability>& configMap)
    {
        MembraneType::PermeabilityMap permeabilityMap;
        for (const auto& [discTypeName, permeability] : configMap)
            permeabilityMap[discTypeRegistry_->getIDFor(discTypeName)] = permeability;

        return permeabilityMap;
    };

    const auto& cellMembraneType = simulationConfig.setup.cellMembraneType;
    types.emplace_back(cellMembraneType.name, cellMembraneType.radius,
                       convertPermeabilityMap(cellMembraneType.permeabilityMap));

    for (const auto& type : simulationConfig.membraneTypes)
        types.emplace_back(type.name, type.radius, convertPermeabilityMap(type.permeabilityMap));

    registry.setValues(std::move(types));

    return registry;
}

ReactionTable SimulationFactory::buildReactionTable(const SimulationConfig& simulationConfig,
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

Cell SimulationFactory::buildCell(const SimulationConfig& simulationConfig) const
{
    std::vector<Membrane> membranes = getMembranesFromConfig(simulationConfig);
    SimulationContext context = getSimulationContext();
    Membrane cellMembrane(membraneTypeRegistry_->getIDFor(simulationConfig.setup.cellMembraneType.name));
    cellMembrane.setPosition({0, 0});

    Cell cell(std::move(cellMembrane), std::move(membranes), std::move(context));

    return cell;
}

std::vector<Membrane> SimulationFactory::getMembranesFromConfig(const SimulationConfig& simulationConfig_) const
{
    std::vector<Membrane> membranes;

    for (const auto& membrane : simulationConfig_.setup.membranes)
    {
        Membrane newMembrane(membraneTypeRegistry_->getIDFor(membrane.membraneTypeName));
        newMembrane.setPosition({membrane.x, membrane.y});

        membranes.push_back(std::move(newMembrane));
    }

    return membranes;
}

void SimulationFactory::throwIfNotBuildYet() const
{
    if (!built_)
        throw ExceptionWithLocation("Simulation context was not yet fully built");
}

bool SimulationFactory::isBuilt() const
{
    return built_;
}

} // namespace cell