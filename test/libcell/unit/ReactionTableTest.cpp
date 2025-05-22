#include "ReactionTable.hpp"
#include "MathUtils.hpp"
#include "Reaction.hpp"
#include "TestUtils.hpp"

#include <gtest/gtest.h>

TEST(ReactionTableTest, LookupMapsWork)
{
    Reaction transformation{Mass5Radius5, std::nullopt, Mass5Radius10, std::nullopt, 1.f};
    Reaction decomposition{Mass10, std::nullopt, Mass5, Mass5, 1.f};
    Reaction combination{Mass5, Mass5, Mass10, std::nullopt, 1.f};
    Reaction exchange{Mass5, Mass15, Mass10, Mass10, 1.f};

    ReactionTable reactionTable;
    reactionTable.setReactions({transformation, decomposition, combination, exchange});

    EXPECT_EQ(reactionTable.getTransformationReactionLookupMap().size(), 1);
    EXPECT_EQ(reactionTable.getTransformationReactionLookupMap().at(Mass5Radius5).front(), transformation);

    EXPECT_EQ(reactionTable.getDecompositionReactionLookupMap().size(), 1);
    EXPECT_EQ(reactionTable.getDecompositionReactionLookupMap().at(Mass10).front(), decomposition);

    EXPECT_EQ(reactionTable.getCombinationReactionLookupMap().size(), 1);
    EXPECT_EQ(reactionTable.getCombinationReactionLookupMap().at(std::make_pair(Mass5, Mass5)).front(), combination);

    EXPECT_EQ(reactionTable.getExchangeReactionLookupMap().size(), 2);
    EXPECT_EQ(reactionTable.getExchangeReactionLookupMap().at(std::make_pair(Mass5, Mass15)).front(), exchange);
    EXPECT_EQ(reactionTable.getExchangeReactionLookupMap().at(std::make_pair(Mass15, Mass5)), exchange);
}
