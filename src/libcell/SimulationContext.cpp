#include "SimulationContext.hpp"
#include "Settings.hpp"

namespace cell
{

SimulationContext::SimulationContext()
    : discTypeResolver_([&](DiscTypeID discTypeID) -> const DiscType& { return discTypeRegistry_.getByID(discTypeID); })
    , reactionTable_(discTypeResolver_)
    , reactionEngine_(discTypeResolver_, reactionTable_.getDecompositionReactionLookupMap(),
                      reactionTable_.getTransformationReactionLookupMap(),
                      reactionTable_.getCombinationReactionLookupMap(), reactionTable_.getExchangeReactionLookupMap())
    , collisionDetector_(discTypeResolver_, [&]() -> double { return discTypeRegistry_.getMaxRadius(); })
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

} // namespace cell