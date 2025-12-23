#include "cell/Cell.hpp"
#include "TestUtils.hpp"
#include "cell/Disc.hpp"
#include "cell/Membrane.hpp"
#include "cell/MembraneType.hpp"
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
    const double timeStep = 1;
    SimulationConfigBuilder builder;
    SimulationFactory simulationFactory;

    void SetUp() override
    {
        builder.addDiscType("A", Radius{5}, Mass{1});
        builder.addDiscType("B", Radius{5}, Mass{1});
        builder.addDiscType("C", Radius{5}, Mass{2});
        builder.addDiscType("D", Radius{5}, Mass{1});
        builder.useDistribution(false);
    }

    Cell& createAndUpdateCell()
    {
        simulationFactory.buildSimulationFromConfig(builder.getSimulationConfig());
        auto& cell = simulationFactory.getCell();
        cell.update(timeStep);

        return cell;
    }

    const DiscTypeRegistry& getDiscTypeRegistry()
    {
        return simulationFactory.getSimulationContext().discTypeRegistry;
    }

    DiscTypeID getIDFor(const std::string& name)
    {
        return getDiscTypeRegistry().getIDFor(name);
    };

    std::vector<Disc> getAllDiscs(const Compartment& compartment)
    {
        std::vector<Disc> discs;
        std::vector<const Compartment*> compartments({&compartment});

        while (!compartments.empty())
        {
            const auto* child = compartments.back();
            compartments.pop_back();

            discs.insert(discs.end(), child->getDiscs().begin(), child->getDiscs().end());

            for (const auto& subCompartment : child->getCompartments())
                compartments.push_back(subCompartment.get());
        }

        return discs;
    }

    const Disc& getDisc(const std::vector<Disc>& discs, const std::string& typeName)
    {
        auto iter = std::find_if(discs.begin(), discs.end(), [&](const Disc& d)
                                 { return getDiscTypeRegistry().getByID(d.getTypeID()).getName() == typeName; });
        if (iter == discs.end())
            throw ExceptionWithLocation("Couldn't find type " + typeName + " in discs");

        return *iter;
    };
};

TEST_F(ACell, SimulatesASingleDisc)
{
    builder.addDisc("A", Position{.x = 50, .y = 50}, Velocity{.x = 1, .y = 1});

    auto& cell = createAndUpdateCell();

    ASSERT_THAT(cell.getDiscs().size(), Eq(1u));
    ASSERT_THAT(cell.getDiscs().front().getPosition().x, DoubleNear(50 + timeStep, MaxPositionError));
    ASSERT_THAT(cell.getDiscs().front().getPosition().y, DoubleNear(50 + timeStep, MaxPositionError));

    ASSERT_THAT(simulationFactory.getAndResetCollisionCounts().empty(), Eq(true));
}

TEST_F(ACell, SimulatesUnimolecularReactions)
{
    builder.addDisc("A", Position{.x = 50, .y = 50}, Velocity{.x = 1, .y = 1});
    builder.addDisc("C", Position{.x = 10, .y = 10}, Velocity{.x = 1, .y = 1});

    builder.addReaction("A", "", "B", "", Probability{1});
    builder.addReaction("C", "", "A", "B", Probability{1});

    auto& cell = createAndUpdateCell();

    auto discTypeCounts = countDiscTypes(cell.getDiscs(), getDiscTypeRegistry());

    ASSERT_THAT(discTypeCounts["A"], Eq(1));
    ASSERT_THAT(discTypeCounts["B"], Eq(2));
    ASSERT_THAT(discTypeCounts["C"], Eq(0));

    auto collisionCounts = simulationFactory.getAndResetCollisionCounts();
    auto getIDFor = [&](const std::string& name) { return getDiscTypeRegistry().getIDFor(name); };

    ASSERT_THAT(collisionCounts[getIDFor("A")], Eq(0));
    ASSERT_THAT(collisionCounts[getIDFor("B")], Eq(0));
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

    auto discTypeCounts = countDiscTypes(cell.getDiscs(), getDiscTypeRegistry());

    EXPECT_THAT(discTypeCounts["A"], Eq(0));
    EXPECT_THAT(discTypeCounts["B"], Eq(1));
    EXPECT_THAT(discTypeCounts["C"], Eq(2));

    auto collisionCounts = simulationFactory.getAndResetCollisionCounts();

    EXPECT_THAT(collisionCounts[getIDFor("A")], Eq(2));
    EXPECT_THAT(collisionCounts[getIDFor("B")], Eq(1));
    EXPECT_THAT(collisionCounts[getIDFor("C")], Eq(1));
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
    builder.addDisc("A", Position{.x = 400, .y = 500}, Velocity{.x = 10, .y = 0});  // Left
    builder.addDisc("B", Position{.x = 500, .y = 400}, Velocity{.x = 0, .y = 10});  // Top
    builder.addDisc("C", Position{.x = 600, .y = 500}, Velocity{.x = -10, .y = 0}); // Right
    builder.addDisc("D", Position{.x = 500, .y = 600}, Velocity{.x = 0, .y = -10}); // Bottom

    // TODO tests for Inside -> outside

    auto& cell = createAndUpdateCell();
    auto discs = getAllDiscs(cell);

    expectNear(getDisc(discs, "A").getPosition(), {410, 500}); // Inward: Should go through
    expectNear(getDisc(discs, "B").getPosition(), {500, 390}); // Outward: Should collide
    expectNear(getDisc(discs, "C").getPosition(), {590, 500}); // Bidirectional: Should go through
    expectNear(getDisc(discs, "D").getPosition(), {500, 610}); // Undefined permeability: should collide
}

TEST_F(ACell, SimulatesReactionsOfDiscsInDifferentCompartments)
{
    builder.addMembraneType(
        "M", Radius{100}, {{"A", MembraneType::Permeability::Bidirectional}, {"B", MembraneType::Permeability::None}});

    builder.addMembrane("M", Position{.x = 0, .y = 0});

    // A outside of M, B is inside
    builder.addDisc("A", Position{.x = 103, .y = 0}, Velocity{.x = -10, .y = 0});
    builder.addDisc("B", Position{.x = 95, .y = 0}, Velocity{.x = 0, .y = 0});

    builder.addReaction("A", "B", "C", "", Probability{1});

    auto& cell = createAndUpdateCell();
    auto discs = getAllDiscs(cell);

    ASSERT_EQ(discs.size(), 1);
    EXPECT_EQ(getDiscTypeRegistry().getByID(discs.front().getTypeID()).getName(), "C");

    const auto& collisionCounts = simulationFactory.getAndResetCollisionCounts();

    ASSERT_EQ(collisionCounts.size(), 2);
    EXPECT_TRUE(collisionCounts.contains(getIDFor("A")) && collisionCounts.at(getIDFor("A")) == 1);
    EXPECT_TRUE(collisionCounts.contains(getIDFor("B")) && collisionCounts.at(getIDFor("B")) == 1);
}

TEST_F(ACell, SimulatesNewDiscsCorrectly)
{
    builder.addDisc("A", Position{.x = 90, .y = 0}, Velocity{.x = -10, .y = 0});
    builder.addDisc("B", Position{.x = 90, .y = 0}, Velocity{.x = 0, .y = 0});
    builder.addDisc("D", Position{.x = 80, .y = 0}, Velocity{.x = 0, .y = 0});

    builder.addReaction("A", "B", "C", "", Probability{1});

    auto& cell = createAndUpdateCell();
    auto discs = getAllDiscs(cell);
    simulationFactory.getAndResetCollisionCounts(); // Discard the collision between A and B

    ASSERT_EQ(discs.size(), 2);
    auto discC = getDisc(discs, "C");
    expectNear(discC.getPosition(), {85, 0});

    cell.update(timeStep);
    discs = getAllDiscs(cell);

    auto collisions = simulationFactory.getAndResetCollisionCounts();
    ASSERT_EQ(collisions.size(), 2);
    ASSERT_TRUE(collisions.contains(getDiscTypeRegistry().getIDFor("C")));
    ASSERT_TRUE(collisions.contains(getDiscTypeRegistry().getIDFor("D")));
}

TEST_F(ACell, SimulatesDiscsPassingThroughMembranesInBothDirections)
{
    builder.addMembraneType(
        "M", Radius{100},
        {{"A", MembraneType::Permeability::Bidirectional}, {"B", MembraneType::Permeability::Bidirectional}});
    builder.addMembrane("M", Position{.x = 0, .y = 0});

    // Collision between disc moving from parent to child compartment with a disc in the child compartment
    builder.addDisc("A", Position{.x = -106, .y = 0}, Velocity{.x = 8, .y = 0}); // Parent -> child
    builder.addDisc("A", Position{.x = -94, .y = 0}, Velocity{.x = 0, .y = 0});  // Child

    // Collision between disc moving from child to parent compartment with a disc in the parent compartment
    builder.addDisc("A", Position{.x = 94, .y = 0}, Velocity{.x = 8, .y = 0});  // Child -> parent
    builder.addDisc("A", Position{.x = 106, .y = 0}, Velocity{.x = 0, .y = 0}); // Parent

    builder.addReaction("A", "A", "C", "", Probability{1});

    auto& cell = createAndUpdateCell();
    auto discs = getAllDiscs(cell);
    EXPECT_EQ(discs.size(), 4);

    cell.update(timeStep);
    discs = getAllDiscs(cell);
    EXPECT_EQ(discs.size(), 2);

    auto counts = countDiscTypes(discs, getDiscTypeRegistry());
    ASSERT_TRUE(counts.contains("C"));
    EXPECT_EQ(counts["C"], 2);
}
