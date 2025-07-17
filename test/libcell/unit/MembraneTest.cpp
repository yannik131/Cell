#include <gtest/gtest.h>

#include "Disc.hpp"
#include "MathUtils.hpp"
#include "Membrane.hpp"
#include "TestUtils.hpp"

using namespace cell;

TEST(MembraneTest, MembranesAreSemipermeable)
{
    // Default position at 0, 0
    Disc d1{Mass5Radius5}, d2{Mass10Radius10}, d3{Mass15Radius10}, d4{Mass20};

    // Default position at 0, 0
    Membrane membrane;
    const int radius = 100;
    membrane.setRadius(radius);
    membrane.setPermeability(Mass5Radius5, Membrane::PermeableInward);
    membrane.setPermeability(Mass10Radius10, Membrane::PermeableOutward);
    membrane.setPermeability(Mass15Radius10, Membrane::PermeableBidirectional);

    EXPECT_EQ(membrane.getPermeability(Mass5Radius5), Membrane::PermeableInward);
    EXPECT_EQ(membrane.getPermeability(Mass10Radius10), Membrane::PermeableOutward);
    EXPECT_EQ(membrane.getPermeability(Mass15Radius10), Membrane::PermeableBidirectional);
}

TEST(MembraneTest, InvalidValuesThrow)
{
    Membrane membrane;

    for (int i = -1; i <= 0; ++i)
    {
        EXPECT_ANY_THROW(membrane.setThickness(i));
        EXPECT_ANY_THROW(membrane.setRadius(i));
    }
}

TEST(MembraneTest, ValidValuesDontThrow)
{
    Membrane membrane;

    EXPECT_NO_THROW(membrane.setThickness(1));
    EXPECT_NO_THROW(membrane.setRadius(1));
}