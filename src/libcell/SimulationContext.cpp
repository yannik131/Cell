#include "SimulationContext.hpp"
#include "Settings.hpp"

namespace cell
{

SimulationContext::SimulationContext()
    : reactionTable_([&](DiscTypeID discTypeID) -> const DiscType& { return discTypeRegistry_.getByID(discTypeID); })
    , reactionEngine_([&](DiscTypeID discTypeID) -> const DiscType& { return discTypeRegistry_.getByID(discTypeID); },
                      reactionTable_.getDecompositionReactionLookupMap(),
                      reactionTable_.getTransformationReactionLookupMap(),
                      reactionTable_.getCombinationReactionLookupMap(), reactionTable_.getExchangeReactionLookupMap())
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