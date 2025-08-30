#include "Settings.hpp"
#include "SimulationConfigBuilder.hpp"
#include "SimulationContext.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace testing;
using namespace cell;

namespace
{

const double MaxPositionError = 1e-9;

std::map<std::string, int> countDiscTypes(const std::vector<Disc>& discs, DiscTypeResolver discTypeResolver)
{
    std::map<std::string, int> counts;
    for (const auto& disc : discs)
        counts[discTypeResolver(disc.getDiscTypeID()).getName()]++;

    return counts;
}

} // namespace

class ACell : public Test
{
protected:
    double timeStep = SettingsLimits::MaxSimulationTimeStep.asSeconds();
    SimulationConfigBuilder builder;
    SimulationContext simulationContext;

    void SetUp() override
    {
        builder.addDiscType("A", Radius{5}, Mass{1});
        builder.addDiscType("B", Radius{5}, Mass{1});
        builder.addDiscType("C", Radius{5}, Mass{2});
        builder.setCellDimensions(Width{100}, Height{100});
        builder.setTimeStep(timeStep);
    }
};

TEST_F(ACell, SimulatesASingleDisc)
{
    builder.addDisc("A", Position{50, 50}, Velocity{1, 1});

    simulationContext.buildContextFromConfig(builder.getSimulationConfig());

    auto& cell = simulationContext.getCell();
    cell.update();

    ASSERT_THAT(cell.getDiscs().size(), Eq(1u));
    ASSERT_THAT(cell.getDiscs().front().getPosition().x, DoubleNear(50 + timeStep, MaxPositionError));
    ASSERT_THAT(cell.getDiscs().front().getPosition().y, DoubleNear(50 + timeStep, MaxPositionError));

    ASSERT_THAT(simulationContext.getAndResetCollisionCounts().empty(), Eq(true));
}

TEST_F(ACell, SimulatesUnimolecularReactions)
{
    builder.addDisc("A", Position{50, 50}, Velocity{1, 1});
    builder.addDisc("C", Position{10, 10}, Velocity{1, 1});

    builder.addReaction("A", "", "B", "", Probability{1});
    builder.addReaction("C", "", "A", "B", Probability{1});

    simulationContext.buildContextFromConfig(builder.getSimulationConfig());

    auto& cell = simulationContext.getCell();
    cell.update();

    auto discTypeCounts =
        countDiscTypes(cell.getDiscs(), simulationContext.getDiscTypeRegistry().getDiscTypeResolver());

    ASSERT_THAT(discTypeCounts["A"], Eq(1));
    ASSERT_THAT(discTypeCounts["B"], Eq(2));
    ASSERT_THAT(discTypeCounts["C"], Eq(0));
}

TEST_F(ACell, SimulatesBimolecularReactions)
{
    builder.addDisc("A", Position{50, 50}, Velocity{1, 1});
    builder.addDisc("B", Position{51, 51}, Velocity{-1, -1});

    builder.addDisc("A", Position{10, 10}, Velocity{1, 1});
    builder.addDisc("C", Position{11, 11}, Velocity{-1, -1});

    builder.addReaction("A", "B", "C", "", Probability{1});
    builder.addReaction("A", "C", "B", "C", Probability{1});

    simulationContext.buildContextFromConfig(builder.getSimulationConfig());

    auto& cell = simulationContext.getCell();
    cell.update();

    auto discTypeCounts =
        countDiscTypes(cell.getDiscs(), simulationContext.getDiscTypeRegistry().getDiscTypeResolver());

    ASSERT_THAT(discTypeCounts["A"], Eq(0));
    ASSERT_THAT(discTypeCounts["B"], Eq(1));
    ASSERT_THAT(discTypeCounts["C"], Eq(2));

    auto collisionCounts = simulationContext.getAndResetCollisionCounts();
    auto getIDFor = [&](const std::string& name) { return simulationContext.getDiscTypeRegistry().getIDFor(name); };

    ASSERT_THAT(collisionCounts[getIDFor("A")], Eq(2));
    ASSERT_THAT(collisionCounts[getIDFor("B")], Eq(1));
    ASSERT_THAT(collisionCounts[getIDFor("C")], Eq(1));
}