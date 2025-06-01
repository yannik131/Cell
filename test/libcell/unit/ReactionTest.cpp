#include "Reaction.hpp"
#include "TestUtils.hpp"

#include <gtest/gtest.h>

#include <cmath>

TEST(ReactionTest, invalidReactions)
{
    Reaction wrongMasses{Mass5, Mass5, Mass10, Mass5, 0.1f};
    EXPECT_ANY_THROW(wrongMasses.validate());

    Reaction invalidTransformation{Mass5, std::nullopt, Mass5, std::nullopt, 0.1f};
    EXPECT_ANY_THROW(invalidTransformation.validate());

    EXPECT_NO_THROW(invalidTransformation.setProbability(0));
    EXPECT_NO_THROW(invalidTransformation.setProbability(1));
    EXPECT_ANY_THROW(invalidTransformation.setProbability(std::nextafter(1.f, 2.f)));
    EXPECT_ANY_THROW(invalidTransformation.setProbability(std::nextafter(0.f, -1.f)));
}

void testReactionGettersSetters(Reaction& reaction, std::vector<bool> availableParts)
{
    auto* reactionPtr = &reaction;

    std::vector<std::function<DiscType()>> getters{
        [reactionPtr] { return reactionPtr->getEduct1(); }, [reactionPtr] { return reactionPtr->getEduct2(); },
        [reactionPtr] { return reactionPtr->getProduct1(); }, [reactionPtr] { return reactionPtr->getProduct2(); }};

    std::vector<std::function<void(const DiscType&)>> setters{
        [reactionPtr](const DiscType& value) { reactionPtr->setEduct1(value); }, [reactionPtr](const DiscType& value)
        { reactionPtr->setEduct2(value); }, [reactionPtr](const DiscType& value) { reactionPtr->setProduct1(value); },
        [reactionPtr](const DiscType& value) { reactionPtr->setProduct2(value); }};

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
    Reaction transformation{Mass5, std::nullopt, Mass5Radius10, std::nullopt, 0.1f};
    Reaction decomposition{Mass10, std::nullopt, Mass5, Mass5, 0.1f};
    Reaction combination{Mass5, Mass5, Mass10, std::nullopt, 0.1f};
    Reaction exchange{Mass5, Mass5, Mass5, Mass5, 0.1f};

    EXPECT_EQ(transformation.getType(), Reaction::Transformation);
    EXPECT_EQ(decomposition.getType(), Reaction::Decomposition);
    EXPECT_EQ(combination.getType(), Reaction::Combination);
    EXPECT_EQ(exchange.getType(), Reaction::Exchange);

    testReactionGettersSetters(transformation, {true, false, true, false});
    testReactionGettersSetters(decomposition, {true, false, true, true});
    testReactionGettersSetters(combination, {true, true, true, false});
    testReactionGettersSetters(exchange, {true, true, true, true});
}

TEST(ReactionTest, testEquality)
{
    Reaction transformation1{Mass5, std::nullopt, Mass5Radius10, std::nullopt, 0.1f};
    Reaction decomposition1{Mass10, std::nullopt, Mass5, Mass5, 0.1f};
    Reaction combination1{Mass5, Mass5, Mass10, std::nullopt, 0.1f};
    Reaction exchange1{Mass5, Mass5, Mass5, Mass5, 0.1f};

    Reaction transformation2{Mass10, std::nullopt, Mass5Radius10, std::nullopt, 0.1f};
    Reaction decomposition2{Mass5, std::nullopt, Mass5, Mass5, 0.1f};
    Reaction combination2{Mass10, Mass5, Mass10, std::nullopt, 0.1f};
    Reaction exchange2{Mass10, Mass5, Mass5, Mass5, 0.1f};

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

TEST(ReactionTest, probabilitiesAddUp)
{
    Reaction transformation1{Mass5, std::nullopt, Mass5Radius10, std::nullopt, 0.1f};
    Reaction transformation2{Mass10, std::nullopt, Mass5Radius10, std::nullopt, 0.1f};
    Reaction transformation3{Mass15, std::nullopt, Mass5Radius10, std::nullopt, 0.9f};
    Reaction combination{Mass5, Mass5, Mass10, std::nullopt, 0.1f};

    std::vector<Reaction> transformations;
    addReactionToVector(transformations, transformation1);
    addReactionToVector(transformations, transformation2);

    EXPECT_FLOAT_EQ(transformations[0].getProbability(), 0.1f);
    EXPECT_FLOAT_EQ(transformations[1].getProbability(), 0.2f);

    EXPECT_ANY_THROW(addReactionToVector(transformations, combination));
    EXPECT_ANY_THROW(addReactionToVector(transformations, transformation1));
    EXPECT_ANY_THROW(addReactionToVector(transformations, transformation3));
}
