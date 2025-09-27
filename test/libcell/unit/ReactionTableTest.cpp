#include "cell/ReactionTable.hpp"
#include "cell/ExceptionWithLocation.hpp"

#include <gtest/gtest.h>

using namespace cell;

class AReactionTable : public ::testing::Test
{
protected:
    DiscTypeRegistry registry;
    std::unique_ptr<ReactionTable> table;
    DiscTypeID A{}, B{}, C{};

    void SetUp() override
    {
        std::vector<DiscType> types;
        types.emplace_back("A", Radius{5}, Mass{1});
        types.emplace_back("B", Radius{5}, Mass{1});
        types.emplace_back("C", Radius{5}, Mass{2});

        registry.setValues(std::move(types));

        table = std::make_unique<ReactionTable>(registry);

        A = registry.getIDFor("A");
        B = registry.getIDFor("B");
        C = registry.getIDFor("C");
    }
};

TEST_F(AReactionTable, ThrowsOnDuplicateTransformation)
{
    Reaction r(A, std::nullopt, B, std::nullopt, 1.0);

    table->addReaction(r);

    EXPECT_THROW(table->addReaction(r), ExceptionWithLocation);
}

TEST_F(AReactionTable, ThrowsOnDuplicateDecomposition)
{
    Reaction r(C, std::nullopt, A, B, 1.0);

    table->addReaction(r);

    EXPECT_THROW(table->addReaction(r), ExceptionWithLocation);
}

TEST_F(AReactionTable, ThrowsOnDuplicateCombination)
{
    Reaction r(A, B, C, std::nullopt, 1.0);

    table->addReaction(r);

    EXPECT_THROW(table->addReaction(r), ExceptionWithLocation);
}

TEST_F(AReactionTable, ThrowsOnDuplicateExchange)
{
    Reaction r(C, B, C, A, 1.0);

    table->addReaction(r);

    EXPECT_THROW(table->addReaction(r), ExceptionWithLocation);
}