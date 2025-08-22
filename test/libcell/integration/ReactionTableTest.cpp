#include "ReactionTable.hpp"
#include "MathUtils.hpp"
#include "Reaction.hpp"
#include "TestUtils.hpp"

#include <gtest/gtest.h>

TEST(ReactionTableTest, lookupMapsWork)
{
    cell::Reaction transformation, decomposition, combination, exchange;
    std::tie(transformation, decomposition, combination, exchange) = getDefaultReactions();

    cell::ReactionTable reactionTable;
    reactionTable.setReactions({transformation, decomposition, combination, exchange});

    EXPECT_EQ(reactionTable.getTransformationReactionLookupMap().size(), 1);
    EXPECT_EQ(reactionTable.getTransformationReactionLookupMap().at(&Mass5Radius5).front(), transformation);

    EXPECT_EQ(reactionTable.getDecompositionReactionLookupMap().size(), 1);
    EXPECT_EQ(reactionTable.getDecompositionReactionLookupMap().at(&Mass10).front(), decomposition);

    EXPECT_EQ(reactionTable.getCombinationReactionLookupMap().size(), 1);
    EXPECT_EQ(reactionTable.getCombinationReactionLookupMap().at(std::make_pair(&Mass5, &Mass5)).front(), combination);

    EXPECT_EQ(reactionTable.getExchangeReactionLookupMap().size(), 2);
    EXPECT_EQ(reactionTable.getExchangeReactionLookupMap().at(std::make_pair(&Mass5, &Mass15)).front(), exchange);
    EXPECT_EQ(reactionTable.getExchangeReactionLookupMap().at(std::make_pair(&Mass15, &Mass5)).front(), exchange);
}

TEST(ReactionTableTest, probabilitiesAddUp)
{
    cell::Reaction transformation1{&Mass5, nullptr, &Mass5Radius10, nullptr, 0.1};
    cell::Reaction transformation2{&Mass10, nullptr, &Mass5Radius10, nullptr, 0.1};
    cell::Reaction transformation3{&Mass15, nullptr, &Mass5Radius10, nullptr, 0.9};
    cell::Reaction combination{&Mass5, &Mass5, &Mass10, nullptr, 0.1};

    std::vector<cell::Reaction> transformations;
    addReactionToVector(transformations, transformation1);
    addReactionToVector(transformations, transformation2);

    EXPECT_DOUBLE_EQ(transformations[0].getProbability(), 0.1);
    EXPECT_DOUBLE_EQ(transformations[1].getProbability(), 0.2);

    EXPECT_ANY_THROW(addReactionToVector(transformations, combination));
    EXPECT_ANY_THROW(addReactionToVector(transformations, transformation1));
    EXPECT_ANY_THROW(addReactionToVector(transformations, transformation3));
}