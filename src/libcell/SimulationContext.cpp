#include "SimulationContext.hpp"
#include "Settings.hpp"

namespace cell
{

SimulationContext::SimulationContext()
    : discTypeResolver_([&](DiscTypeID discTypeID) -> const DiscType& { return discTypeRegistry_.getByID(discTypeID); })
    , maxRadiusProvider_([&]() -> double { return discTypeRegistry_.getMaxRadius(); })
    , reactionTable_(discTypeResolver_)
    , reactionEngine_(discTypeResolver_, reactionTable_.getDecompositionReactionLookupMap(),
                      reactionTable_.getTransformationReactionLookupMap(),
                      reactionTable_.getCombinationReactionLookupMap(), reactionTable_.getExchangeReactionLookupMap())
    , collisionDetector_(discTypeResolver_, maxRadiusProvider_)
    , collisionHandler_(discTypeResolver_)
{
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

const sf::Time& SimulationContext::getSimulationTimeStep() const
{
    return simulationTimeStep_;
}

double SimulationContext::getSimulationTimeScale() const
{
    return simulationTimeScale_;
}

DiscTypeResolver SimulationContext::getDiscTypeResolver() const
{
    return discTypeResolver_;
}

std::function<double()> SimulationContext::getMaxRadiusProvider() const
{
    return maxRadiusProvider_;
}

const ReactionEngine* SimulationContext::getReactionEngine() const
{
    return &reactionEngine_;
}

const CollisionDetector* SimulationContext::getCollisionDetector() const
{
    return &collisionDetector_;
}

const CollisionHandler* SimulationContext::getCollisionHandler() const
{
    return &collisionHandler_;
}

} // namespace cell