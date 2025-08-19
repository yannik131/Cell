#include "Disc.hpp"
#include "MathUtils.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace testing;
using namespace cell;

static DiscType defaultType()
{
    return {"A", sf::Color::White, 5.0, 5.0};
}

class ADisc : public Test
{
protected:
    static const DiscType Type;
    Disc disc{&Type};
};

const DiscType ADisc::Type = defaultType();

TEST_F(ADisc, HasNoMomentumByDefault)
{
    ASSERT_THAT(disc.getAbsoluteMomentum(), DoubleEq(0.0));
    ASSERT_THAT(disc.getMomentum(), Eq(sf::Vector2d{}));
}

TEST_F(ADisc, HasNoKineticEnergyByDefault)
{
    ASSERT_THAT(disc.getKineticEnergy(), DoubleEq(0.0));
}

TEST_F(ADisc, IsPositionedAtOriginByDefault)
{
    ASSERT_THAT(disc.getPosition(), Eq(sf::Vector2d{}));
}

TEST_F(ADisc, HasNoVelocityByDefault)
{
    ASSERT_THAT(disc.getVelocity(), Eq(sf::Vector2d{}));
}

TEST_F(ADisc, IsNotMarkedDestroyedByDefault)
{
    ASSERT_THAT(disc.isMarkedDestroyed(), Eq(false));
}

TEST_F(ADisc, HasTheTypeItWasInitializedWith)
{
    ASSERT_THAT(disc.getType(), Eq(&Type));
}

TEST_F(ADisc, CanBeSetToADifferentPosition)
{
    sf::Vector2d newPosition{10, 10};

    disc.setPosition(newPosition);

    ASSERT_THAT(disc.getPosition(), Eq(newPosition));
}

TEST_F(ADisc, CanBeMoved)
{
    sf::Vector2d dPosition{5, 5};

    disc.setPosition(
        dPosition); // Make sure move is not equal to setPosition by having an initial position unequal the origin
    disc.move(dPosition);

    ASSERT_THAT(disc.getPosition(), Eq(2.0 * dPosition));
}

TEST_F(ADisc, CanBeAssignedANewDiscType)
{
    DiscType B{"B", sf::Color::Blue, 5.0, 3.0};

    disc.setType(&B);

    ASSERT_THAT(disc.getType(), Eq(&B));
}

TEST_F(ADisc, CanBeMarkedDestroyed)
{
    disc.markDestroyed();

    ASSERT_THAT(disc.isMarkedDestroyed(), Eq(true));
}

class ADiscWithVelocity : public ADisc
{
protected:
    const sf::Vector2d Velocity{3.0, 4.0};

    void SetUp() override
    {
        disc.setVelocity(Velocity);
    }
};

TEST_F(ADiscWithVelocity, HasAVelocity)
{
    ASSERT_THAT(disc.getVelocity(), Eq(Velocity));
}

TEST_F(ADiscWithVelocity, VelocityCanBeScaled)
{
    const double Factor = 2.0;

    disc.scaleVelocity(Factor);

    ASSERT_THAT(disc.getVelocity(), Eq(Velocity * Factor));
}

TEST_F(ADiscWithVelocity, CanBeAccelerated)
{
    sf::Vector2d accel{2.0, 3.0};

    disc.accelerate(accel);

    ASSERT_THAT(disc.getVelocity(), Eq(Velocity + accel));
}

TEST_F(ADiscWithVelocity, CanHaveItsXVelocityNegated)
{
    disc.negateXVelocity();

    EXPECT_THAT(disc.getVelocity(), Eq(sf::Vector2d{-Velocity.x, Velocity.y}));
}

TEST_F(ADiscWithVelocity, CanHaveItsYVelocityNegated)
{
    disc.negateYVelocity();

    EXPECT_THAT(disc.getVelocity(), Eq(sf::Vector2d{Velocity.x, -Velocity.y}));
}

TEST_F(ADiscWithVelocity, CalculatesTheCorrectMomentum)
{
    ASSERT_THAT(disc.getMomentum(), Eq(sf::Vector2d{9.0, 15.0}));
}

TEST_F(ADiscWithVelocity, CalculatesTheCorrectAbsoluteMomentum)
{
    ASSERT_THAT(disc.getAbsoluteMomentum(), DoubleEq(25.0));
}

TEST_F(ADiscWithVelocity, CalculatesTheCorrectKineticEnergy)
{
    ASSERT_THAT(disc.getKineticEnergy(), 0.5 * 5.0 * 5.0);
}