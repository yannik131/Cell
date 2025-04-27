#include "DiscType.hpp" // Adjust include as needed
#include <SFML/Graphics/Color.hpp>
#include <gtest/gtest.h>

TEST(DiscTypeTest, ConstructorAndGetters)
{
    DiscType dt("TestDisc", sf::Color::Blue, 5.0f, 10.0f);
    EXPECT_EQ(dt.getName(), "TestDisc");
    EXPECT_EQ(dt.getColor(), sf::Color::Blue);
    EXPECT_FLOAT_EQ(dt.getRadius(), 5.0f);
    EXPECT_FLOAT_EQ(dt.getMass(), 10.0f);
}

TEST(DiscTypeTest, Setters)
{
    DiscType dt("Test", sf::Color::Red, 1.0f, 2.0f);

    dt.setName("Updated");
    EXPECT_EQ(dt.getName(), "Updated");

    dt.setColor(sf::Color::Green);
    EXPECT_EQ(dt.getColor(), sf::Color::Green);

    dt.setRadius(3.3f);
    EXPECT_FLOAT_EQ(dt.getRadius(), 3.3f);

    dt.setMass(4.4f);
    EXPECT_FLOAT_EQ(dt.getMass(), 4.4f);
}

TEST(DiscTypeTest, SettersThrow)
{
    DiscType dt("Test", sf::Color::Red, 1.0f, 2.0f);

    EXPECT_ANY_THROW(dt.setColor(sf::Color::Black));

    for (float i = -1; i <= 0.f; ++i)
    {
        EXPECT_ANY_THROW(dt.setMass(i));
        EXPECT_ANY_THROW(dt.setRadius(i));
    }

    EXPECT_ANY_THROW(dt.setName(""));
}

TEST(DiscTypeTest, CopyConstructor)
{
    DiscType dt1("CopyTest", sf::Color::Yellow, 2.5f, 5.5f);
    DiscType dt2(dt1);

    EXPECT_EQ(dt1.getId(), dt2.getId());
    EXPECT_EQ(dt2.getName(), "CopyTest");
}

TEST(DiscTypeTest, AssignmentOperator)
{
    DiscType dt1("First", sf::Color::White, 1.0f, 1.0f);
    DiscType dt2("Second", sf::Color::Blue, 2.0f, 2.0f);

    dt2 = dt1;
    EXPECT_EQ(dt1, dt2);
}

TEST(DiscTypeTest, EqualityOperator)
{
    DiscType dt1("Equal", sf::Color::Blue, 3.0f, 6.0f);
    DiscType dt2("Equal", sf::Color::Blue, 3.0f, 6.0f);

    // Don't have same ID
    EXPECT_FALSE(dt1 == dt2);
}

TEST(DiscTypeTest, LessOperator)
{
    DiscType dt1("Alpha", sf::Color::Blue, 3.0f, 6.0f);
    DiscType dt2("Beta", sf::Color::Red, 4.0f, 8.0f);

    EXPECT_TRUE(dt1 < dt2);
    EXPECT_FALSE(dt2 < dt1);
}

TEST(DiscTypeTest, MakeOrderedPair)
{
    DiscType dt2("Alpha", sf::Color::Magenta, 4.0f, 8.0f);
    DiscType dt1("Zeta", sf::Color::Cyan, 3.0f, 6.0f);

    auto orderedPair = makeOrderedPair(dt1, dt2);

    EXPECT_EQ(orderedPair.first.getName(), "Alpha");
    EXPECT_EQ(orderedPair.second.getName(), "Zeta");
}