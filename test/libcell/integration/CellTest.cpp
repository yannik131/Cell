#include "SimulationConfigBuilder.hpp"
#include "SimulationContext.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace testing;
using namespace cell;

const double MaxPositionError = 1e-9;

TEST(ACell, CorrectlySimulatesASingleDisc)
{
    Position position{50, 50};
    Velocity velocity{1, 1};
    const double timeStep = 1e-3;

    SimulationConfigBuilder builder;
    builder.addDisc("A", position, velocity);
    builder.addDiscType("A", Radius{5.0}, Mass{1.0});
    builder.setCellDimensions(Width{100}, Height{100});
    builder.setTimeStep(timeStep);

    SimulationContext simulationContext;
    simulationContext.buildContextFromConfig(builder.getSimulationConfig());

    auto& cell = simulationContext.getCell();
    cell.update();

    ASSERT_THAT(cell.getDiscs().size(), Eq(1u));
    ASSERT_THAT(cell.getDiscs().front().getPosition().x,
                DoubleNear(position.x + velocity.x * timeStep, MaxPositionError));
    ASSERT_THAT(cell.getDiscs().front().getPosition().y,
                DoubleNear(position.y + velocity.y * timeStep, MaxPositionError));

    ASSERT_THAT(cell.getAndResetCollisionCount().empty(), Eq(true));
}