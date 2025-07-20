#include <gtest/gtest.h>

#include "Disc.hpp"
#include "MathUtils.hpp"
#include "TestUtils.hpp"
#include "types/MembraneType.hpp"

using namespace cell;

TEST(MembraneTypeTest, MembranesAreSemipermeable)
{
    // Default position at 0, 0
    Disc d1{Mass5Radius5}, d2{Mass10Radius10}, d3{Mass15Radius10}, d4{Mass20};

    // Default position at 0, 0
    MembraneType membrane;
    const int radius = 100;
    membrane.setRadius(radius);
    membrane.setPermeability(Mass5Radius5, MembraneType::PermeableInward);
    membrane.setPermeability(Mass10Radius10, MembraneType::PermeableOutward);
    membrane.setPermeability(Mass15Radius10, MembraneType::PermeableBidirectional);

    EXPECT_EQ(membrane.getPermeability(Mass5Radius5), MembraneType::PermeableInward);
    EXPECT_EQ(membrane.getPermeability(Mass10Radius10), MembraneType::PermeableOutward);
    EXPECT_EQ(membrane.getPermeability(Mass15Radius10), MembraneType::PermeableBidirectional);
}

TEST(MembraneTypeTest, InvalidValuesThrow)
{
    MembraneType membrane;

    for (int i = -1; i <= 0; ++i)
    {
        EXPECT_ANY_THROW(membrane.setThickness(i));
        EXPECT_ANY_THROW(membrane.setRadius(i));
    }
}

TEST(MembraneTypeTest, ValidValuesDontThrow)
{
    MembraneType membrane;

    EXPECT_NO_THROW(membrane.setThickness(1));
    EXPECT_NO_THROW(membrane.setRadius(1));
}