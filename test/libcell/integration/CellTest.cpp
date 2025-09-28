#include "TestUtils.hpp"
#include "cell/Disc.hpp"
#include "cell/Membrane.hpp"
#include "cell/MembraneType.hpp"
#include "cell/Settings.hpp"
#include "cell/SimulationConfigBuilder.hpp"
#include "cell/SimulationFactory.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace testing;
using namespace cell;

namespace
{

const double MaxPositionError = 1e-9;

} // namespace

class ACell : public Test
{
protected:
    double timeStep = 1;
    SimulationConfigBuilder builder;
    SimulationFactory simulationContext;

    void SetUp() override
    {
        builder.addDiscType("A", Radius{5}, Mass{1});
        builder.addDiscType("B", Radius{5}, Mass{1});
        builder.addDiscType("C", Radius{5}, Mass{2});
        builder.addDiscType("D", Radius{5}, Mass{1});
        builder.useDistribution(false);
        builder.setCellDimensions(Width{1000}, Height{1000});
    }

    Cell& createAndUpdateCell()
    {
        simulationContext.buildSimulationFromConfig(builder.getSimulationConfig());
        auto& cell = simulationContext.getCell();
        cell.update(timeStep);

        return cell;
    }
};

TEST_F(ACell, SimulatesASingleDisc)
{
    builder.addDisc("A", Position{.x = 50, .y = 50}, Velocity{.x = 1, .y = 1});

    auto& cell = createAndUpdateCell();

    ASSERT_THAT(cell.getDiscs().size(), Eq(1u));
    ASSERT_THAT(cell.getDiscs().front().getPosition().x, DoubleNear(50 + timeStep, MaxPositionError));
    ASSERT_THAT(cell.getDiscs().front().getPosition().y, DoubleNear(50 + timeStep, MaxPositionError));

    ASSERT_THAT(simulationContext.getAndResetCollisionCounts().empty(), Eq(true));
}

TEST_F(ACell, SimulatesUnimolecularReactions)
{
    builder.addDisc("A", Position{.x = 50, .y = 50}, Velocity{.x = 1, .y = 1});
    builder.addDisc("C", Position{.x = 10, .y = 10}, Velocity{.x = 1, .y = 1});

    builder.addReaction("A", "", "B", "", Probability{1});
    builder.addReaction("C", "", "A", "B", Probability{1});

    auto& cell = createAndUpdateCell();

    auto discTypeCounts = countDiscTypes(cell.getDiscs(), simulationContext.getDiscTypeRegistry());

    ASSERT_THAT(discTypeCounts["A"], Eq(1));
    ASSERT_THAT(discTypeCounts["B"], Eq(2));
    ASSERT_THAT(discTypeCounts["C"], Eq(0));

    auto collisionCounts = simulationContext.getAndResetCollisionCounts();
    auto getIDFor = [&](const std::string& name) { return simulationContext.getDiscTypeRegistry().getIDFor(name); };

    ASSERT_THAT(collisionCounts[getIDFor("A")], Eq(1));
    ASSERT_THAT(collisionCounts[getIDFor("B")], Eq(1));
    ASSERT_THAT(collisionCounts[getIDFor("C")], Eq(0));
}

TEST_F(ACell, SimulatesBimolecularReactions)
{
    builder.addDisc("A", Position{.x = 50, .y = 50}, Velocity{.x = 1, .y = 1});
    builder.addDisc("B", Position{.x = 51, .y = 51}, Velocity{.x = -1, .y = -1});

    builder.addDisc("A", Position{.x = 10, .y = 10}, Velocity{.x = 1, .y = 1});
    builder.addDisc("C", Position{.x = 11, .y = 11}, Velocity{.x = -1, .y = -1});

    builder.addReaction("A", "B", "C", "", Probability{1});
    builder.addReaction("A", "C", "B", "C", Probability{1});

    auto& cell = createAndUpdateCell();

    auto discTypeCounts = countDiscTypes(cell.getDiscs(), simulationContext.getDiscTypeRegistry());

    ASSERT_THAT(discTypeCounts["A"], Eq(0));
    ASSERT_THAT(discTypeCounts["B"], Eq(1));
    ASSERT_THAT(discTypeCounts["C"], Eq(2));

    auto collisionCounts = simulationContext.getAndResetCollisionCounts();
    auto getIDFor = [&](const std::string& name) { return simulationContext.getDiscTypeRegistry().getIDFor(name); };

    ASSERT_THAT(collisionCounts[getIDFor("A")], Eq(2));
    ASSERT_THAT(collisionCounts[getIDFor("B")], Eq(1));
    ASSERT_THAT(collisionCounts[getIDFor("C")], Eq(1));
}

TEST_F(ACell, SimulatesASingleMembrane)
{
    builder.addMembraneType("M", Radius{100},
                            {{"A", MembraneType::Permeability::Inward},
                             {"B", MembraneType::Permeability::Outward},
                             {"C", MembraneType::Permeability::Bidirectional}});

    // Left: (400, 500), Top: (500, 400), Right: (600, 500), Bottom: (500, 600)
    builder.addMembrane("M", Position{.x = 500, .y = 500});

    // Outside -> inside
    builder.addDisc("A", Position{.x = 390, .y = 500}, Velocity{.x = 20, .y = 0});  // Left
    builder.addDisc("B", Position{.x = 500, .y = 390}, Velocity{.x = 0, .y = 20});  // Top
    builder.addDisc("C", Position{.x = 610, .y = 500}, Velocity{.x = -20, .y = 0}); // Right
    builder.addDisc("D", Position{.x = 500, .y = 610}, Velocity{.x = 0, .y = -20}); // Bottom

    auto& cell = createAndUpdateCell();
    const auto& discs = cell.getDiscs();

    const auto& getDisc = [&](const std::string& typeName)
    {
        auto iter = std::find_if(
            discs.begin(), discs.end(), [&](const Disc& d)
            { return simulationContext.getDiscTypeRegistry().getByID(d.getDiscTypeID()).getName() == typeName; });
        if (iter == discs.end())
            throw std::logic_error("Couldn't find type " + typeName + " in discs");

        return *iter;
    };

    expectNear(getDisc("A").getPosition(), {410, 500});
    expectNear(getDisc("B").getPosition(), {500, 390});
    expectNear(getDisc("C").getPosition(), {590, 500});
    expectNear(getDisc("D").getPosition(), {500, 610}); // Undefined permeability, should collide
}