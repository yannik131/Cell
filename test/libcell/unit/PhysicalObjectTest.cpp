#include "PhysicalObject.hpp"

#include <gtest/gtest.h>

TEST(PhysicalObjectTest, DefaultValuesMakeSense)
{
    cell::PhysicalObject physicalObject;

    EXPECT_EQ(physicalObject.getAbsoluteMomentum(), 0);
    EXPECT_EQ(physicalObject.getKineticEnergy(), 0);
    EXPECT_EQ(physicalObject.getPosition(), (sf::Vector2d{0, 0}));
    EXPECT_EQ(physicalObject.getVelocity(), (sf::Vector2d{0, 0}));
}

TEST(PhysicalObjectTest, SetVelocityUpdatesVelocity)
{
    cell::PhysicalObject physicalObject;

    sf::Vector2d vel{3., 4.};
    physicalObject.setVelocity(vel);
    EXPECT_EQ(physicalObject.getVelocity(), vel);
}

TEST(PhysicalObjectTest, ScaleVelocityScalesVelocity)
{
    cell::PhysicalObject physicalObject;

    sf::Vector2d vel{2., 3.};
    physicalObject.setVelocity(vel);
    double factor = 2.0f;
    physicalObject.scaleVelocity(factor);

    EXPECT_DOUBLE_EQ(physicalObject.getVelocity().x, vel.x * factor);
    EXPECT_DOUBLE_EQ(physicalObject.getVelocity().y, vel.y * factor);
}

TEST(PhysicalObjectTest, AccelerateAddsAccelerationToVelocity)
{
    cell::PhysicalObject physicalObject;

    sf::Vector2d initialVel{1., 1.};
    physicalObject.setVelocity(initialVel);

    sf::Vector2d accel{2., 3.};
    physicalObject.accelerate(accel);

    EXPECT_DOUBLE_EQ(physicalObject.getVelocity().x, initialVel.x + accel.x);
    EXPECT_DOUBLE_EQ(physicalObject.getVelocity().y, initialVel.y + accel.y);
}

TEST(PhysicalObjectTest, NegateXVelocityReversesXComponent)
{
    cell::PhysicalObject physicalObject;

    sf::Vector2d vel{5., 3.};
    physicalObject.setVelocity(vel);
    physicalObject.negateXVelocity();

    EXPECT_DOUBLE_EQ(physicalObject.getVelocity().x, -vel.x);
    EXPECT_DOUBLE_EQ(physicalObject.getVelocity().y, vel.y);
}

TEST(PhysicalObjectTest, NegateYVelocityReversesYComponent)
{
    cell::PhysicalObject physicalObject;

    sf::Vector2d vel{5., 3.};
    physicalObject.setVelocity(vel);
    physicalObject.negateYVelocity();

    EXPECT_DOUBLE_EQ(physicalObject.getVelocity().x, vel.x);
    EXPECT_DOUBLE_EQ(physicalObject.getVelocity().y, -vel.y);
}

TEST(PhysicalObjectTest, SetPositionUpdatesPosition)
{
    cell::PhysicalObject physicalObject;

    sf::Vector2d pos{100., 200.};
    physicalObject.setPosition(pos);

    EXPECT_DOUBLE_EQ(physicalObject.getPosition().x, pos.x);
    EXPECT_DOUBLE_EQ(physicalObject.getPosition().y, pos.y);
}

TEST(PhysicalObjectTest, MoveAdjustsPosition)
{
    cell::PhysicalObject physicalObject;

    sf::Vector2d initialPos{50., 50.};
    physicalObject.setPosition(initialPos);

    sf::Vector2d distance{10., 15.};
    physicalObject.move(distance);

    EXPECT_DOUBLE_EQ(physicalObject.getPosition().x, initialPos.x + distance.x);
    EXPECT_DOUBLE_EQ(physicalObject.getPosition().y, initialPos.y + distance.y);
}

TEST(PhysicalObjectTest, GetAbsoluteMomentumCalculatesCorrectly)
{
    cell::PhysicalObject physicalObject;
    double mass = 5;

    physicalObject.setMass(mass);

    sf::Vector2d vel{3., 4.}; // magnitude = sqrt(9 + 16) = 5
    physicalObject.setVelocity(vel);

    double expectedMomentum = mass * 5.;

    EXPECT_DOUBLE_EQ(physicalObject.getAbsoluteMomentum(), expectedMomentum);
}

TEST(PhysicalObjectTest, GetKineticEnergyCalculatesCorrectly)
{
    cell::PhysicalObject physicalObject;
    double mass = 6;

    physicalObject.setMass(mass);

    sf::Vector2d vel{3., 4.}; // speed = 5, v^2 = 25
    physicalObject.setVelocity(vel);

    double expectedKE = 0.5f * mass * 25.;
    EXPECT_DOUBLE_EQ(physicalObject.getKineticEnergy(), expectedKE);
}