#include "cell/Compartment.hpp"
#include "TestUtils.hpp"
#include "cell/Cell.hpp"
#include "cell/MathUtils.hpp"
#include "cell/SimulationConfigBuilder.hpp"
#include "cell/SimulationFactory.hpp"

#include <gtest/gtest.h>

using namespace testing;
using namespace cell;

namespace
{
const Compartment& getSingleChildCompartment(const Compartment& compartment)
{
    const auto& compartments = compartment.getCompartments();

    if (compartments.size() != 1)
        throw std::invalid_argument("Given compartment has not 1 child compartment, it has " +
                                    std::to_string(compartments.size()));

    return *compartments.front();
}

bool notContainedInOthers(const Compartment& host, const std::vector<const Compartment*>& others,
                          const SimulationContext& context)
{
    // d = disc, h = host, o = other, M = circle center, R = radius
    const auto Mh = host.getMembrane().getPosition();
    const auto Rh = context.membraneTypeRegistry.getByID(host.getMembrane().getTypeID()).getRadius();

    for (const auto& disc : host.getDiscs())
    {
        const auto Md = disc.getPosition();
        const auto Rd = context.discTypeRegistry.getByID(disc.getTypeID()).getRadius();

        if (!mathutils::circleIsFullyContainedByCircle(Md, Rd, Mh, Rh))
            return false;

        // The disc center may be inside another compartment, but it shouldn't be fully contained by it
        // That could only occur after updating a few times (a disc is only "ejected" from a compartment once it fully
        // left)
        for (const auto& compartment : others)
        {
            const auto Mo = compartment->getMembrane().getPosition();
            const auto& Ro = context.membraneTypeRegistry.getByID(compartment->getMembrane().getTypeID()).getRadius();

            if (mathutils::circleIsFullyContainedByCircle(Md, Rd, Mo, Ro))
                return false;
        }
    }

    return true;
}

} // namespace

class ACompartment : public Test
{
protected:
    SimulationConfigBuilder builder;
    SimulationFactory simulationFactory;

    void SetUp() override
    {
        builder.addDiscType("A", Radius{1}, Mass{1});
        builder.addDiscType("B", Radius{1}, Mass{1});

        builder.addMembraneType("Small", Radius{50}, {});
        builder.addMembraneType("Large", Radius{200}, {});
        builder.addMembrane("Small", Position{.x = 500, .y = 500});
        builder.addMembrane("Large", Position{.x = 500, .y = 500});
    }

    auto& getCell()
    {
        simulationFactory.buildSimulationFromConfig(builder.getSimulationConfig());
        return simulationFactory.getCell();
    }

    const DiscTypeRegistry& getDiscTypeRegistry()
    {
        return simulationFactory.getSimulationContext().discTypeRegistry;
    }

    double getRadius(const Compartment& compartment)
    {
        return simulationFactory.getSimulationContext()
            .membraneTypeRegistry.getByID(compartment.getMembrane().getTypeID())
            .getRadius();
    }
};

TEST_F(ACompartment, CanHaveDiscAssignedDirectly)
{
    builder.useDistribution(false);

    builder.addDisc("A", Position{.x = 500, .y = 500}, {});
    builder.addDisc("A", Position{.x = 550, .y = 500}, {}); // Not contained by Small -> in Large
    builder.addDisc("B", Position{.x = 600, .y = 500}, {});

    auto& cell = getCell();
    const auto& largeCompartment = getSingleChildCompartment(cell);
    const auto& smallCompartment = getSingleChildCompartment(largeCompartment);

    // Make sure compartments are correct
    EXPECT_EQ(getRadius(largeCompartment), 200);
    EXPECT_EQ(getRadius(smallCompartment), 50);

    // Parents
    EXPECT_EQ(cell.getParent(), nullptr);
    EXPECT_EQ(largeCompartment.getParent(), &cell);
    EXPECT_EQ(smallCompartment.getParent(), &largeCompartment);

    // Disc counts
    ASSERT_TRUE(cell.getDiscs().empty());
    ASSERT_EQ(largeCompartment.getDiscs().size(), 2);
    ASSERT_EQ(smallCompartment.getDiscs().size(), 1);

    // Disc positions
    for (const auto& disc : largeCompartment.getDiscs())
    {
        if (disc.getPosition().x != 550 && disc.getPosition().x != 600)
            FAIL() << "Large compartment should contain the 2 discs at x=550 and x=600";
    }

    expectNear(smallCompartment.getDiscs().front().getPosition(), {500, 500});
}

TEST_F(ACompartment, CanHaveADiscDistribution)
{
    const std::size_t N = 100;
    builder.setDiscCount("", static_cast<int>(N));
    builder.setDiscCount("Large", static_cast<int>(N));
    builder.setDiscCount("Small", static_cast<int>(N));

    builder.setDistribution("", {{"A", 0.5}, {"B", 0.5}});
    builder.setDistribution("Large", {{"A", 1}, {"B", 0}});
    builder.setDistribution("Small", {{"A", 0}, {"B", 1}});

    auto& cell = getCell();
    const auto& largeCompartment = getSingleChildCompartment(cell);
    const auto& smallCompartment = getSingleChildCompartment(largeCompartment);

    // Disc counts
    ASSERT_EQ(cell.getDiscs().size(), N);
    ASSERT_EQ(largeCompartment.getDiscs().size(), N);
    ASSERT_EQ(smallCompartment.getDiscs().size(), N);

    // Disc type counts
    auto countsCell = countDiscTypes(cell.getDiscs(), getDiscTypeRegistry());
    auto countsLarge = countDiscTypes(largeCompartment.getDiscs(), getDiscTypeRegistry());
    auto countsSmall = countDiscTypes(smallCompartment.getDiscs(), getDiscTypeRegistry());

    EXPECT_EQ(countsCell["A"], N / 2);
    EXPECT_EQ(countsCell["B"], N / 2);
    EXPECT_EQ(countsLarge["A"], N);
    EXPECT_EQ(countsSmall["B"], N);

    // Disc containment
    auto context = simulationFactory.getSimulationContext();
    ASSERT_TRUE(notContainedInOthers(cell, {&largeCompartment, &smallCompartment}, context));
    ASSERT_TRUE(notContainedInOthers(largeCompartment, {&smallCompartment}, context));
}

TEST_F(ACompartment, CantOverlapWithAnotherCompartment)
{
    builder.addMembraneType("M", Radius{100}, {});
    builder.addMembrane("M", Position{.x = 500, .y = 500});
    builder.addMembrane("M", Position{.x = 500, .y = 500});

    ASSERT_ANY_THROW(getCell());
}

TEST_F(ACompartment, MustBeLargerThanTheLargestDiscType)
{
    builder.addDiscType("Large", Radius{50}, Mass{1});
    builder.addMembraneType("M", Radius{10}, {});
    builder.addMembrane("M", Position{.x = 500, .y = 500});

    ASSERT_ANY_THROW(getCell());
}