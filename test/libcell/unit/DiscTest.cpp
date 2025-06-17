#include "Disc.hpp"

#include <gtest/gtest.h>

static cell::DiscType defaultType()
{
    return {"A", sf::Color::White, 5.f, 5.f};
}

static const cell::DiscType Type = defaultType();

TEST(DiscTest, DefaultValuesMakeSense)
{
    cell::Disc disc(Type);

    EXPECT_EQ(disc.getAbsoluteMomentum(), 0);
    EXPECT_EQ(disc.getKineticEnergy(), 0);
    EXPECT_EQ(disc.getPosition(), (sf::Vector2f{0, 0}));
    EXPECT_EQ(disc.getType(), Type);
    EXPECT_EQ(disc.getVelocity(), (sf::Vector2f{0, 0}));
    EXPECT_EQ(disc.isMarkedDestroyed(), false);
}

TEST(DiscTest, SetVelocityUpdatesVelocity)
{
    cell::Disc disc(Type);

    sf::Vector2f vel{3.f, 4.f};
    disc.setVelocity(vel);
    EXPECT_EQ(disc.getVelocity(), vel);
}

TEST(DiscTest, ScaleVelocityScalesVelocity)
{
    cell::Disc disc(Type);

    sf::Vector2f vel{2.f, 3.f};
    disc.setVelocity(vel);
    float factor = 2.0f;
    disc.scaleVelocity(factor);

    EXPECT_FLOAT_EQ(disc.getVelocity().x, vel.x * factor);
    EXPECT_FLOAT_EQ(disc.getVelocity().y, vel.y * factor);
}

TEST(DiscTest, AccelerateAddsAccelerationToVelocity)
{
    cell::Disc disc(Type);

    sf::Vector2f initialVel{1.f, 1.f};
    disc.setVelocity(initialVel);

    sf::Vector2f accel{2.f, 3.f};
    disc.accelerate(accel);

    EXPECT_FLOAT_EQ(disc.getVelocity().x, initialVel.x + accel.x);
    EXPECT_FLOAT_EQ(disc.getVelocity().y, initialVel.y + accel.y);
}

TEST(DiscTest, NegateXVelocityReversesXComponent)
{
    cell::Disc disc(Type);

    sf::Vector2f vel{5.f, 3.f};
    disc.setVelocity(vel);
    disc.negateXVelocity();

    EXPECT_FLOAT_EQ(disc.getVelocity().x, -vel.x);
    EXPECT_FLOAT_EQ(disc.getVelocity().y, vel.y);
}

TEST(DiscTest, NegateYVelocityReversesYComponent)
{
    cell::Disc disc(Type);

    sf::Vector2f vel{5.f, 3.f};
    disc.setVelocity(vel);
    disc.negateYVelocity();

    EXPECT_FLOAT_EQ(disc.getVelocity().x, vel.x);
    EXPECT_FLOAT_EQ(disc.getVelocity().y, -vel.y);
}

TEST(DiscTest, SetPositionUpdatesPosition)
{
    cell::Disc disc(Type);

    sf::Vector2f pos{100.f, 200.f};
    disc.setPosition(pos);

    EXPECT_FLOAT_EQ(disc.getPosition().x, pos.x);
    EXPECT_FLOAT_EQ(disc.getPosition().y, pos.y);
}

TEST(DiscTest, MoveAdjustsPosition)
{
    cell::Disc disc(Type);

    sf::Vector2f initialPos{50.f, 50.f};
    disc.setPosition(initialPos);

    sf::Vector2f distance{10.f, 15.f};
    disc.move(distance);

    EXPECT_FLOAT_EQ(disc.getPosition().x, initialPos.x + distance.x);
    EXPECT_FLOAT_EQ(disc.getPosition().y, initialPos.y + distance.y);
}

TEST(DiscTest, SetTypeUpdatesDiscType)
{
    cell::DiscType newType{"B", sf::Color::Green, 4.f, 4.f};

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

    sf::Vector2f vel{3.f, 4.f}; // magnitude = sqrt(9 + 16) = 5
    disc.setVelocity(vel);

    float expectedMomentum = Type.getMass() * 5.f;

    EXPECT_FLOAT_EQ(disc.getAbsoluteMomentum(), expectedMomentum);
}

TEST(DiscTest, GetKineticEnergyCalculatesCorrectly)
{
    cell::Disc disc(Type);

    sf::Vector2f vel{3.f, 4.f}; // speed = 5, v^2 = 25
    disc.setVelocity(vel);

    float expectedKE = 0.5f * Type.getMass() * 25.f;
    EXPECT_FLOAT_EQ(disc.getKineticEnergy(), expectedKE);
}