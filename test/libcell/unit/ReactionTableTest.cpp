#include "ReactionTable.hpp"
#include "DiscTypeRegistry.hpp"
#include "ExceptionWithLocation.hpp"

#include <gtest/gtest.h>

using namespace cell;

class ReactionTableTest : public ::testing::Test
{
protected:
    DiscTypeRegistry registry;
    DiscTypeResolver resolver;
    DiscTypeID A, B, C;

    void SetUp() override
    {
        std::vector<DiscType> types;
        types.emplace_back("A", Radius{5}, Mass{1});
        types.emplace_back("B", Radius{5}, Mass{1});
        types.emplace_back("C", Radius{5}, Mass{2});

        registry.setDiscTypes(std::move(types));
        resolver = registry.getDiscTypeResolver();

        A = registry.getIDFor("A");
        B = registry.getIDFor("B");
        C = registry.getIDFor("C");
    }
};

TEST_F(ReactionTableTest, ThrowsOnDuplicateTransformation)
{
    ReactionTable table(resolver);
    Reaction r(A, std::nullopt, B, std::nullopt, 1.0);

    table.addReaction(r);

    EXPECT_THROW(table.addReaction(r), ExceptionWithLocation);
}

TEST_F(ReactionTableTest, ThrowsOnDuplicateDecomposition)
{
    ReactionTable table(resolver);
    Reaction r(C, std::nullopt, A, B, 1.0);

    table.addReaction(r);

    EXPECT_THROW(table.addReaction(r), ExceptionWithLocation);
}

TEST_F(ReactionTableTest, ThrowsOnDuplicateCombination)
{
    ReactionTable table(resolver);
    Reaction r(A, B, C, std::nullopt, 1.0);

    table.addReaction(r);

    EXPECT_THROW(table.addReaction(r), ExceptionWithLocation);
}

TEST_F(ReactionTableTest, ThrowsOnDuplicateExchange)
{
    ReactionTable table(resolver);
    Reaction r(C, B, C, A, 1.0);

    table.addReaction(r);

    EXPECT_THROW(table.addReaction(r), ExceptionWithLocation);
}