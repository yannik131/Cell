#include "Disc.hpp"

#include <gtest/gtest.h>

static const DiscType Type{"A", sf::Color::White, 5.f, 5.f};

TEST(DiscTest, DefaultValuesMakeSense)
{
    Disc disc(Type);

    EXPECT_EQ(disc.getAbsoluteMomentum(), 0);
    EXPECT_EQ(disc.getKineticEnergy(), 0);
    EXPECT_EQ(disc.getPosition(), (sf::Vector2f{0, 0}));
    EXPECT_EQ(disc.getType(), Type);
    EXPECT_EQ(disc.getVelocity(), (sf::Vector2f{0, 0}));
    EXPECT_EQ(disc.isMarkedDestroyed(), false);
}

TEST(DiscTest, SetVelocityUpdatesVelocity)
{
    Disc disc(Type);

    sf::Vector2f vel{3.f, 4.f};
    disc.setVelocity(vel);
    EXPECT_EQ(disc.getVelocity(), vel);
}

TEST(DiscTest, ScaleVelocityScalesVelocity)
{
    Disc disc(Type);

    sf::Vector2f vel{2.f, 3.f};
    disc.setVelocity(vel);
    float factor = 2.0f;
    disc.scaleVelocity(factor);

    EXPECT_FLOAT_EQ(disc.getVelocity().x, vel.x * factor);
    EXPECT_FLOAT_EQ(disc.getVelocity().y, vel.y * factor);
}

TEST(DiscTest, AccelerateAddsAccelerationToVelocity)
{
    Disc disc(Type);

    sf::Vector2f initialVel{1.f, 1.f};
    disc.setVelocity(initialVel);

    sf::Vector2f accel{2.f, 3.f};
    disc.accelerate(accel);

    EXPECT_FLOAT_EQ(disc.getVelocity().x, initialVel.x + accel.x);
    EXPECT_FLOAT_EQ(disc.getVelocity().y, initialVel.y + accel.y);
}

TEST(DiscTest, NegateXVelocityReversesXComponent)
{
    Disc disc(Type);

    sf::Vector2f vel{5.f, 3.f};
    disc.setVelocity(vel);
    disc.negateXVelocity();

    EXPECT_FLOAT_EQ(disc.getVelocity().x, -vel.x);
    EXPECT_FLOAT_EQ(disc.getVelocity().y, vel.y);
}

TEST(DiscTest, NegateYVelocityReversesYComponent)
{
    Disc disc(Type);

    sf::Vector2f vel{5.f, 3.f};
    disc.setVelocity(vel);
    disc.negateYVelocity();

    EXPECT_FLOAT_EQ(disc.getVelocity().x, vel.x);
    EXPECT_FLOAT_EQ(disc.getVelocity().y, -vel.y);
}

TEST(DiscTest, SetPositionUpdatesPosition)
{
    Disc disc(Type);

    sf::Vector2f pos{100.f, 200.f};
    disc.setPosition(pos);

    EXPECT_FLOAT_EQ(disc.getPosition().x, pos.x);
    EXPECT_FLOAT_EQ(disc.getPosition().y, pos.y);
}

TEST(DiscTest, MoveAdjustsPosition)
{
    Disc disc(Type);

    sf::Vector2f initialPos{50.f, 50.f};
    disc.setPosition(initialPos);

    sf::Vector2f distance{10.f, 15.f};
    disc.move(distance);

    EXPECT_FLOAT_EQ(disc.getPosition().x, initialPos.x + distance.x);
    EXPECT_FLOAT_EQ(disc.getPosition().y, initialPos.y + distance.y);
}

TEST(DiscTest, SetTypeUpdatesDiscType)
{
    DiscType newType{"B", sf::Color::Green, 4.f, 4.f};

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

TEST(DiscTest, GetIdReturnsUniqueId)
{
    Disc disc1(Type);
    Disc disc2(Type);

    EXPECT_NE(disc1.getId(), disc2.getId());
}

TEST(DiscTest, GetAbsoluteMomentumCalculatesCorrectly)
{
    Disc disc(Type);

    sf::Vector2f vel{3.f, 4.f}; // magnitude = sqrt(9 + 16) = 5
    disc.setVelocity(vel);

    float expectedMomentum = Type.getMass() * 5.f;

    EXPECT_FLOAT_EQ(disc.getAbsoluteMomentum(), expectedMomentum);
}

TEST(DiscTest, GetKineticEnergyCalculatesCorrectly)
{
    Disc disc(Type);

    sf::Vector2f vel{3.f, 4.f}; // speed = 5, v^2 = 25
    disc.setVelocity(vel);

    float expectedKE = 0.5f * Type.getMass() * 25.f;
    EXPECT_FLOAT_EQ(disc.getKineticEnergy(), expectedKE);
}