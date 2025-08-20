#include "DiscType.hpp"

#include <SFML/Graphics/Color.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <type_traits>

using namespace testing;
using namespace cell;

class ADiscType : public Test
{
protected:
    DiscType discType{"Test", sf::Color::Red, 1.0, 2.0};
};

TEST_F(ADiscType, IsConstructedCorrectly)
{
    EXPECT_THAT(discType.getName(), Eq("Test"));
    EXPECT_THAT(discType.getColor(), Eq(sf::Color::Red));
    EXPECT_THAT(discType.getRadius(), DoubleEq(1.0));
    EXPECT_THAT(discType.getMass(), DoubleEq(2.0));
}

TEST_F(ADiscType, CanHaveItsNameChanged)
{
    discType.setName("Hello");

    ASSERT_THAT(discType.getName(), Eq("Hello"));
}

TEST_F(ADiscType, CantHaveAnEmptyName)
{
    ASSERT_ANY_THROW(discType.setName(""));
}

TEST_F(ADiscType, CanHaveItsColorChanged)
{
    discType.setColor(sf::Color::Green);

    ASSERT_THAT(discType.getColor(), Eq(sf::Color::Green));
}

TEST_F(ADiscType, CantHaveAnEmptyColor)
{
    ASSERT_ANY_THROW(discType.setColor(sf::Color()));
}

TEST_F(ADiscType, CantBeBlack)
{
    ASSERT_ANY_THROW(discType.setColor(sf::Color::Black));
}

TEST_F(ADiscType, CanHaveItsRadiusChanged)
{
    discType.setRadius(10.0);

    ASSERT_THAT(discType.getRadius(), DoubleEq(10.0));
}

TEST_F(ADiscType, MustHaveAPositiveRadius)
{
    ASSERT_ANY_THROW(discType.setRadius(0));
    ASSERT_ANY_THROW(discType.setRadius(-1.0));
}

TEST_F(ADiscType, CanHaveItsMassChanged)
{
    discType.setMass(10.0);

    ASSERT_THAT(discType.getMass(), DoubleEq(10.0));
}

TEST_F(ADiscType, MustHaveAPositiveMass)
{
    ASSERT_ANY_THROW(discType.setMass(0));
    ASSERT_ANY_THROW(discType.setMass(-1.0));
}

TEST_F(ADiscType, CanBeEqual)
{
    DiscType other{"Test", sf::Color::Red, 1.0, 2.0};

    ASSERT_TRUE(discType == other);
}

TEST_F(ADiscType, CanBeUnequal)
{
    DiscType other{"Fest", sf::Color::Red, 1.0, 2.0};

    ASSERT_FALSE(discType == other);
}

TEST(ADiscType, IsNoncopyable)
{
    static_assert(!std::is_copy_constructible_v<cell::DiscType>);
    static_assert(!std::is_copy_assignable_v<cell::DiscType>);
}