#include "Disc.hpp"
#include "DiscType.hpp"
#include "MathUtils.hpp"
#include "TestUtils.hpp"

#include <gtest/gtest.h>

#include <numbers>

using namespace cell;

TEST(Collision, ConservesEnergy)
{
    DiscType discType("B", sf::Color::Red, 1.0, 1.0);
    Disc d1(discType), d2(discType);

    sf::Vector2d d1InitialPosition{0, 0};
    d1.setPosition(d1InitialPosition);
    d1.setVelocity({500, -500});

    // d2 touches d1 at time t = 0, but no collision yet
    d2.setPosition(d1InitialPosition + sf::Vector2d{0.1f, 0.1f});
    d2.setVelocity({-400, 300});

    const float kineticEnergyBefore = d1.getKineticEnergy() + d2.getKineticEnergy();

    mathutils::handleDiscCollisions({std::make_pair(&d1, &d2)});

    const float kineticEnergyAfter = d1.getKineticEnergy() + d2.getKineticEnergy();

    EXPECT_NEAR(kineticEnergyBefore, kineticEnergyAfter, 1e-4);
}