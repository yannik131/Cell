#include "DistributionAndReactionsModel.hpp"

#include <gtest/gtest.h>

typedef DistributionAndReactionsModel Model;

TEST(DistributionAndReactionsModelTest, FrequencyAddsUpTo100)
{
    Model m;
    m.updateDiscType({.oldName = "", .newName = "A", .radius = 10, .mass = 10, .color = "Red", .frequency = 50});

    ASSERT_THROW(m.getDiscTypeDistribution(), std::runtime_error);

    m.updateDiscType({.oldName = "", .newName = "B", .radius = 10, .mass = 10, .color = "Red", .frequency = 50});

    ASSERT_NO_THROW(m.getDiscTypeDistribution());
}
