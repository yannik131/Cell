#include "Disc.hpp"

#include <gtest/gtest.h>

static cell::DiscType defaultType()
{
    return {"A", sf::Color::White, 5.0, 5.0};
}

static const cell::DiscType Type = defaultType();

TEST(DiscTest, DefaultValuesMakeSense)
{
    cell::Disc disc(Type);

    EXPECT_EQ(disc.getAbsoluteMomentum(), 0);
    EXPECT_EQ(disc.getKineticEnergy(), 0);
    EXPECT_EQ(disc.getPosition(), (sf::Vector2d{0, 0}));
    EXPECT_EQ(disc.getType(), Type);
    EXPECT_EQ(disc.getVelocity(), (sf::Vector2d{0, 0}));
    EXPECT_EQ(disc.isMarkedDestroyed(), false);
}

TEST(DiscTest, SetVelocityUpdatesVelocity)
{
    cell::Disc disc(Type);

    sf::Vector2d vel{3.0, 4.0};
    disc.setVelocity(vel);
    EXPECT_EQ(disc.getVelocity(), vel);
}

TEST(DiscTest, ScaleVelocityScalesVelocity)
{
    cell::Disc disc(Type);

    sf::Vector2d vel{2.0, 3.0};
    disc.setVelocity(vel);
    double factor = 2.0f;
    disc.scaleVelocity(factor);

    EXPECT_FLOAT_EQ(disc.getVelocity().x, vel.x * factor);
    EXPECT_FLOAT_EQ(disc.getVelocity().y, vel.y * factor);
}

TEST(DiscTest, AccelerateAddsAccelerationToVelocity)
{
    cell::Disc disc(Type);

    sf::Vector2d initialVel{1.0, 1.0};
    disc.setVelocity(initialVel);

    sf::Vector2d accel{2.0, 3.0};
    disc.accelerate(accel);

    EXPECT_FLOAT_EQ(disc.getVelocity().x, initialVel.x + accel.x);
    EXPECT_FLOAT_EQ(disc.getVelocity().y, initialVel.y + accel.y);
}

TEST(DiscTest, NegateXVelocityReversesXComponent)
{
    cell::Disc disc(Type);

    sf::Vector2d vel{5.0, 3.0};
    disc.setVelocity(vel);
    disc.negateXVelocity();

    EXPECT_FLOAT_EQ(disc.getVelocity().x, -vel.x);
    EXPECT_FLOAT_EQ(disc.getVelocity().y, vel.y);
}

TEST(DiscTest, NegateYVelocityReversesYComponent)
{
    cell::Disc disc(Type);

    sf::Vector2d vel{5.0, 3.0};
    disc.setVelocity(vel);
    disc.negateYVelocity();

    EXPECT_FLOAT_EQ(disc.getVelocity().x, vel.x);
    EXPECT_FLOAT_EQ(disc.getVelocity().y, -vel.y);
}

TEST(DiscTest, SetPositionUpdatesPosition)
{
    cell::Disc disc(Type);

    sf::Vector2d pos{100.0, 200.0};
    disc.setPosition(pos);

    EXPECT_FLOAT_EQ(disc.getPosition().x, pos.x);
    EXPECT_FLOAT_EQ(disc.getPosition().y, pos.y);
}

TEST(DiscTest, MoveAdjustsPosition)
{
    cell::Disc disc(Type);

    sf::Vector2d initialPos{50.0, 50.0};
    disc.setPosition(initialPos);

    sf::Vector2d distance{10.0, 15.0};
    disc.move(distance);

    EXPECT_FLOAT_EQ(disc.getPosition().x, initialPos.x + distance.x);
    EXPECT_FLOAT_EQ(disc.getPosition().y, initialPos.y + distance.y);
}

TEST(DiscTest, SetTypeUpdatesDiscType)
{
    cell::DiscType newType{"B", sf::Color::Green, 4.0, 4.0};

    cell::Disc disc(Type);
    disc.setType(newType);

    EXPECT_EQ(disc.getType(), newType);
}

TEST(DiscTest, MarkDestroyedSetsFlag)
{
    cell::Disc disc(Type);
    disc.markDestroyed();

    EXPECT_TRUE(disc.isMarkedDestroyed());
}

TEST(DiscTest, GetAbsoluteMomentumCalculatesCorrectly)
{
    cell::Disc disc(Type);

    sf::Vector2d vel{3.0, 4.0}; // magnitude = sqrt(9 + 16) = 5
    disc.setVelocity(vel);

    double expectedMomentum = Type.getMass() * 5.0;

    EXPECT_FLOAT_EQ(disc.getAbsoluteMomentum(), expectedMomentum);
}

TEST(DiscTest, GetKineticEnergyCalculatesCorrectly)
{
    cell::Disc disc(Type);

    sf::Vector2d vel{3.0, 4.0}; // speed = 5, v^2 = 25
    disc.setVelocity(vel);

    double expectedKE = 0.5f * Type.getMass() * 25.0;
    EXPECT_FLOAT_EQ(disc.getKineticEnergy(), expectedKE);
}