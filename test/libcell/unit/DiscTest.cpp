#include "Disc.hpp"

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

TEST(DiscTest, AccelerateAddsAccelerationToVelocity)
{
    Disc disc(Type);

    sf::Vector2d initialVel{1.0, 1.0};
    disc.setVelocity(initialVel);

    sf::Vector2d accel{2.0, 3.0};
    disc.accelerate(accel);

    EXPECT_FLOAT_EQ(disc.getVelocity().x, initialVel.x + accel.x);
    EXPECT_FLOAT_EQ(disc.getVelocity().y, initialVel.y + accel.y);
}

TEST(DiscTest, NegateXVelocityReversesXComponent)
{
    Disc disc(Type);

    sf::Vector2d vel{5.0, 3.0};
    disc.setVelocity(vel);
    disc.negateXVelocity();

    EXPECT_FLOAT_EQ(disc.getVelocity().x, -vel.x);
    EXPECT_FLOAT_EQ(disc.getVelocity().y, vel.y);
}

TEST(DiscTest, NegateYVelocityReversesYComponent)
{
    Disc disc(Type);

    sf::Vector2d vel{5.0, 3.0};
    disc.setVelocity(vel);
    disc.negateYVelocity();

    EXPECT_FLOAT_EQ(disc.getVelocity().x, vel.x);
    EXPECT_FLOAT_EQ(disc.getVelocity().y, -vel.y);
}

TEST(DiscTest, SetPositionUpdatesPosition)
{
    Disc disc(Type);

    sf::Vector2d pos{100.0, 200.0};
    disc.setPosition(pos);

    EXPECT_FLOAT_EQ(disc.getPosition().x, pos.x);
    EXPECT_FLOAT_EQ(disc.getPosition().y, pos.y);
}

TEST(DiscTest, MoveAdjustsPosition)
{
    Disc disc(Type);

    sf::Vector2d initialPos{50.0, 50.0};
    disc.setPosition(initialPos);

    sf::Vector2d distance{10.0, 15.0};
    disc.move(distance);

    EXPECT_FLOAT_EQ(disc.getPosition().x, initialPos.x + distance.x);
    EXPECT_FLOAT_EQ(disc.getPosition().y, initialPos.y + distance.y);
}

TEST(DiscTest, SetTypeUpdatesDiscType)
{
    DiscType newType{"B", sf::Color::Green, 4.0, 4.0};

    Disc disc(Type);
    disc.setType(newType);

    EXPECT_EQ(disc.getType(), newType);
}

TEST(DiscTest, MarkDestroyedSetsFlag)
{
    Disc disc(Type);
    disc.markDestroyed();

    EXPECT_TRUE(disc.isMarkedDestroyed());
}

TEST(DiscTest, GetAbsoluteMomentumCalculatesCorrectly)
{
    Disc disc(Type);

    sf::Vector2d vel{3.0, 4.0}; // magnitude = sqrt(9 + 16) = 5
    disc.setVelocity(vel);

    double expectedMomentum = Type.getMass() * 5.0;

    EXPECT_FLOAT_EQ(disc.getAbsoluteMomentum(), expectedMomentum);
}

TEST(DiscTest, GetKineticEnergyCalculatesCorrectly)
{
    Disc disc(Type);

    sf::Vector2d vel{3.0, 4.0}; // speed = 5, v^2 = 25
    disc.setVelocity(vel);

    double expectedKE = 0.5f * Type.getMass() * 25.0;
    EXPECT_FLOAT_EQ(disc.getKineticEnergy(), expectedKE);
}