#include <gtest/gtest.h>

#include "Disc.hpp"
#include "MathUtils.hpp"
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
    membrane.bePermeableFor(Mass5Radius5, Membrane::permeableInward);
    membrane.bePermeableFor(Mass10Radius10, Membrane::permeableOutward);
    membrane.bePermeableFor(Mass15Radius10, Membrane::permeableBidirectional);

    EXPECT_EQ(membrane.getPermeability(Mass5Radius5), Membrane::permeableInward);
}