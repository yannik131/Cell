#include "SimulationFactory.hpp"
#include "Cell.hpp"
#include "CellPopulator.hpp"
#include "CollisionDetector.hpp"
#include "CollisionHandler.hpp"
#include "Disc.hpp"
#include "Membrane.hpp"
#include "ReactionEngine.hpp"
#include "ReactionTable.hpp"
#include "Settings.hpp"
#include "SimulationContext.hpp"
#include "StringUtils.hpp"

#include <glog/logging.h>

#include <random>

namespace cell
{

SimulationFactory::SimulationFactory() = default;
SimulationFactory::~SimulationFactory() = default;

void SimulationFactory::buildSimulationFromConfig(const SimulationConfig& simulationConfig)
{
    reset();

    try
    {
        discTypeRegistry_ = std::make_unique<DiscTypeRegistry>(buildDiscTypeRegistry(simulationConfig));
        membraneTypeRegistry_ = std::make_unique<MembraneTypeRegistry>(buildMembraneTypeRegistry(simulationConfig));
    }
    catch (const std::exception& e)
    {
        throw InvalidTypesException(e.what());
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
        collisionDetector_ = std::make_unique<CollisionDetector>(std::as_const(*discTypeRegistry_),
                                                                 std::as_const(*membraneTypeRegistry_));
        collisionHandler_ = std::make_unique<CollisionHandler>(std::as_const(*discTypeRegistry_),
                                                               std::as_const(*membraneTypeRegistry_));

        cell_ = buildCell(simulationConfig);
    }
    catch (const std::exception& e)
    {
        throw InvalidSetupException(e.what());
    }
}

SimulationContext SimulationFactory::getSimulationContext()
{
    if (!discTypeRegistry_ || !membraneTypeRegistry_ || !reactionEngine_ || !collisionDetector_ || !collisionHandler_)
        throw ExceptionWithLocation("Can't get simulation context, dependencies haven't been fully created yet");

    return SimulationContext{.discTypeRegistry = *discTypeRegistry_,
                             .membraneTypeRegistry = *membraneTypeRegistry_,
                             .reactionEngine = *reactionEngine_,
                             .collisionDetector = *collisionDetector_,
                             .collisionHandler = *collisionHandler_};
}

Cell& SimulationFactory::getCell()
{
    if (!cell_)
        throw ExceptionWithLocation("Can't access cell, it hasn't been created yet");

    return *cell_;
}

DiscTypeMap<int> SimulationFactory::getAndResetCollisionCounts()
{
    if (!collisionDetector_)
        throw ExceptionWithLocation("Can't access collision detector, it hasn't been created yet");

    return collisionDetector_->getAndResetCollisionCounts();
}

DiscTypeRegistry SimulationFactory::buildDiscTypeRegistry(const SimulationConfig& simulationConfig)
{
    DiscTypeRegistry discTypeRegistry;
    std::vector<DiscType> discTypes;
    for (const auto& discType : simulationConfig.discTypes)
        discTypes.emplace_back(discType.name, Radius{discType.radius}, Mass{discType.mass});

    discTypeRegistry.setValues(std::move(discTypes));

    return discTypeRegistry;
}

MembraneTypeRegistry SimulationFactory::buildMembraneTypeRegistry(const SimulationConfig& simulationConfig)
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
                                                    const DiscTypeRegistry& discTypeRegistry)
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

std::unique_ptr<Cell> SimulationFactory::buildCell(const SimulationConfig& simulationConfig)
{
    const auto& configMembranes = simulationConfig.setup.membranes;
    if (std::find_if(configMembranes.begin(), configMembranes.end(), [&](const auto& membrane)
                     { return membrane.membraneTypeName == config::cellMembraneTypeName; }) != configMembranes.end())
        throw ExceptionWithLocation("There can't be more than 1 cell membrane in the simulation");

    for (const auto& [discTypeID, permeability] : simulationConfig.setup.cellMembraneType.permeabilityMap)
    {
        if (permeability != MembraneType::Permeability::None)
            throw ExceptionWithLocation("Currently the outer cell membrane does not support permeability");
    }

    throwIfDiscsCanBeLargerThanMembranes(simulationConfig);

    std::vector<Membrane> membranes = getMembranesFromConfig(simulationConfig);

    Membrane cellMembrane(membraneTypeRegistry_->getIDFor(config::cellMembraneTypeName));
    cellMembrane.setPosition({0, 0});

    std::unique_ptr<Cell> cell(std::make_unique<Cell>(std::move(cellMembrane), getSimulationContext()));
    createCompartments(*cell, std::move(membranes));

    CellPopulator cellPopulator(*cell, simulationConfig, getSimulationContext());
    cellPopulator.populateCell();

    return cell;
}

std::vector<Membrane> SimulationFactory::getMembranesFromConfig(const SimulationConfig& simulationConfig_)
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

void SimulationFactory::reset()
{
    discTypeRegistry_.reset();
    membraneTypeRegistry_.reset();
    reactionTable_.reset();
    reactionEngine_.reset();
    collisionDetector_.reset();
    collisionHandler_.reset();
    cell_.reset();
}

void SimulationFactory::createCompartments(Cell& cell, std::vector<Membrane> membranes)
{
    // We'll sort ascending by size and convert membranes to compartments from large to small
    // This way, compartments are sorted descending by size, and finding a parent compartment is simple:
    // Iterate from small to large and find the first compartment that contains the current fully

    std::sort(membranes.begin(), membranes.end(),
              [&](const Membrane& lhs, const Membrane& rhs)
              {
                  return membraneTypeRegistry_->getByID(lhs.getTypeID()).getRadius() <
                         membraneTypeRegistry_->getByID(rhs.getTypeID()).getRadius();
              });

    std::vector<Compartment*> compartments({&cell});
    while (!membranes.empty())
    {
        auto membrane = std::move(membranes.back());
        membranes.pop_back();

        const auto& M = membrane.getPosition();
        const auto& R = membraneTypeRegistry_->getByID(membrane.getTypeID()).getRadius();

        bool parentFound = false;
        for (auto iter = compartments.rbegin(); !parentFound && iter != compartments.rend(); ++iter)
        {
            const auto& Mo = (*iter)->getMembrane().getPosition();
            const auto& Ro = membraneTypeRegistry_->getByID((*iter)->getMembrane().getTypeID()).getRadius();

            if (!mathutils::circleIsFullyContainedByCircle(M, R, Mo, Ro))
                continue;

            auto compartment = (*iter)->createSubCompartment(std::move(membrane));
            compartments.push_back(compartment);
            parentFound = true;
        }

        if (!parentFound)
            throw ExceptionWithLocation("Couldn't find a compartment that fully contains the membrane at (" +
                                        std::to_string(M.x) + "," + std::to_string(M.y) + ")");
    }

    // We do it here to include the cell itself in the check
    throwIfCompartmentsIntersect(compartments);
}

void SimulationFactory::throwIfCompartmentsIntersect(const std::vector<Compartment*>& compartments) const
{
    // This is done once, so no worries about O(n^2)
    for (std::size_t i = 0; i < compartments.size(); ++i)
    {
        const auto R1 = membraneTypeRegistry_->getByID(compartments[i]->getMembrane().getTypeID()).getRadius();
        const auto M1 = compartments[i]->getMembrane().getPosition();

        for (std::size_t j = i + 1; j < compartments.size(); ++j)
        {
            const auto R2 = membraneTypeRegistry_->getByID(compartments[j]->getMembrane().getTypeID()).getRadius();
            const auto M2 = compartments[j]->getMembrane().getPosition();

            if (mathutils::circlesIntersect(M1, R1, M2, R2))
                throw ExceptionWithLocation("Overlapping membranes at " + stringutils::toString(M1) + " and " +
                                            stringutils::toString(M2) + " not allowed");
        }
    }
}

void SimulationFactory::throwIfDiscsCanBeLargerThanMembranes(const SimulationConfig& config) const
{
    const auto maxDiscRadius = std::max_element(config.discTypes.begin(), config.discTypes.end(),
                                                [](const auto& t1, const auto& t2) { return t1.radius < t2.radius; })
                                   ->radius;

    auto minMembraneRadius = config.setup.cellMembraneType.radius;
    for (const auto& membraneType : config.membraneTypes)
        minMembraneRadius = std::min(minMembraneRadius, membraneType.radius);

    if (maxDiscRadius >= minMembraneRadius)
        throw ExceptionWithLocation("At least one disc type has a radius larger than at least one membrane type.");
}

} // namespace cell