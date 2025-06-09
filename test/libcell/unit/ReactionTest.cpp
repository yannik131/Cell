#include "Reaction.hpp"
#include "TestUtils.hpp"

#include <gtest/gtest.h>

#include <cmath>

TEST(ReactionTest, invalidReactions)
{
    cell::Reaction wrongMasses{Mass5, Mass5, Mass10, Mass5, 0.1f};
    EXPECT_ANY_THROW(wrongMasses.validate());

    cell::Reaction invalidTransformation{Mass5, std::nullopt, Mass5, std::nullopt, 0.1f};
    EXPECT_ANY_THROW(invalidTransformation.validate());

    EXPECT_NO_THROW(invalidTransformation.setProbability(0));
    EXPECT_NO_THROW(invalidTransformation.setProbability(1));
    EXPECT_ANY_THROW(invalidTransformation.setProbability(std::nextafter(1.f, 2.f)));
    EXPECT_ANY_THROW(invalidTransformation.setProbability(std::nextafter(0.f, -1.f)));
}

void testReactionGettersSetters(cell::Reaction& reaction, std::vector<bool> availableParts)
{
    auto* reactionPtr = &reaction;

    std::vector<std::function<cell::DiscType()>> getters{
        [reactionPtr] { return reactionPtr->getEduct1(); }, [reactionPtr] { return reactionPtr->getEduct2(); },
        [reactionPtr] { return reactionPtr->getProduct1(); }, [reactionPtr] { return reactionPtr->getProduct2(); }};

    std::vector<std::function<void(const cell::DiscType&)>> setters{
        [reactionPtr](const cell::DiscType& value) { reactionPtr->setEduct1(value); },
        [reactionPtr](const cell::DiscType& value) { reactionPtr->setEduct2(value); },
        [reactionPtr](const cell::DiscType& value) { reactionPtr->setProduct1(value); },
        [reactionPtr](const cell::DiscType& value) { reactionPtr->setProduct2(value); }};

    std::vector<std::function<bool()>> checkers{nullptr, [reactionPtr] { return reactionPtr->hasEduct2(); }, nullptr,
                                                [reactionPtr] { return reactionPtr->hasProduct2(); }};

    for (std::size_t i = 0; i < availableParts.size(); ++i)
    {
        if (!availableParts[i])
        {
            EXPECT_ANY_THROW(getters[i]());
            EXPECT_ANY_THROW(setters[i](Mass5));

            if (checkers[i])
                EXPECT_FALSE(checkers[i]());
        }
        else
        {
            EXPECT_NO_THROW(getters[i]());
            EXPECT_NO_THROW(setters[i](Mass5));

            if (checkers[i])
                EXPECT_TRUE(checkers[i]());
        }
    }
}

TEST(ReactionTest, gettersSetters)
{
    cell::Reaction transformation{Mass5, std::nullopt, Mass5Radius10, std::nullopt, 0.1f};
    cell::Reaction decomposition{Mass10, std::nullopt, Mass5, Mass5, 0.1f};
    cell::Reaction combination{Mass5, Mass5, Mass10, std::nullopt, 0.1f};
    cell::Reaction exchange{Mass5, Mass5, Mass5, Mass5, 0.1f};

    EXPECT_EQ(transformation.getType(), cell::Reaction::Transformation);
    EXPECT_EQ(decomposition.getType(), cell::Reaction::Decomposition);
    EXPECT_EQ(combination.getType(), cell::Reaction::Combination);
    EXPECT_EQ(exchange.getType(), cell::Reaction::Exchange);

    testReactionGettersSetters(transformation, {true, false, true, false});
    testReactionGettersSetters(decomposition, {true, false, true, true});
    testReactionGettersSetters(combination, {true, true, true, false});
    testReactionGettersSetters(exchange, {true, true, true, true});
}

TEST(ReactionTest, testEquality)
{
    cell::Reaction transformation1{Mass5, std::nullopt, Mass5Radius10, std::nullopt, 0.1f};
    cell::Reaction decomposition1{Mass10, std::nullopt, Mass5, Mass5, 0.1f};
    cell::Reaction combination1{Mass5, Mass5, Mass10, std::nullopt, 0.1f};
    cell::Reaction exchange1{Mass5, Mass5, Mass5, Mass5, 0.1f};

    cell::Reaction transformation2{Mass10, std::nullopt, Mass5Radius10, std::nullopt, 0.1f};
    cell::Reaction decomposition2{Mass5, std::nullopt, Mass5, Mass5, 0.1f};
    cell::Reaction combination2{Mass10, Mass5, Mass10, std::nullopt, 0.1f};
    cell::Reaction exchange2{Mass10, Mass5, Mass5, Mass5, 0.1f};

    // Self-equality
    EXPECT_EQ(transformation1, transformation1);
    EXPECT_EQ(decomposition1, decomposition1);
    EXPECT_EQ(combination1, combination1);
    EXPECT_EQ(exchange1, exchange1);

    // Variant inequality
    EXPECT_NE(transformation1, transformation2);
    EXPECT_NE(decomposition1, decomposition2);
    EXPECT_NE(combination1, combination2);
    EXPECT_NE(exchange1, exchange2);

    // Cross-type inequality
    EXPECT_NE(transformation1, decomposition1);
    EXPECT_NE(transformation1, combination1);
    EXPECT_NE(transformation1, exchange1);

    EXPECT_NE(decomposition1, transformation1);
    EXPECT_NE(decomposition1, combination1);
    EXPECT_NE(decomposition1, exchange1);

    EXPECT_NE(combination1, transformation1);
    EXPECT_NE(combination1, decomposition1);
    EXPECT_NE(combination1, exchange1);

    EXPECT_NE(exchange1, transformation1);
    EXPECT_NE(exchange1, decomposition1);
    EXPECT_NE(exchange1, combination1);
}
