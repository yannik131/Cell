#include "MathUtils.hpp"
#include "Disc.hpp"
#include "TestUtils.hpp"

#include <glog/logging.h>
#include <gtest/gtest.h>

#include <cmath>
#include <numbers>
#include <unordered_set>

using namespace cell; // Defines the += and /= operators

TEST(MathUtilsTest, OperatorPlusEqualsWorksForMaps)
{
    using namespace mathutils;

    std::unordered_map<int, double> m1{{1, 1.0}, {2, 2.0}};
    std::unordered_map<int, double> m2{{1, 2.0}, {2, 3.0}, {3, 3.0}};

    m1 += m2;

    EXPECT_DOUBLE_EQ(m1[1], 3.0);
    EXPECT_DOUBLE_EQ(m1[2], 5.0);
}

TEST(MathUtilsTest, OperatorDivideEqualsWorksForMaps)
{
    using namespace mathutils;

    std::unordered_map<int, double> m1{{1, 1.0}, {2, 2.0}};
    m1 /= 2.0;

    EXPECT_DOUBLE_EQ(m1[1], 0.5);
    EXPECT_DOUBLE_EQ(m1[2], 1.0);
}

TEST(MathUtilsTest, FindsCollidingDiscs)
{
    Disc d1(&Mass5), d2(&Mass5);
    d1.setPosition({0, 0});
    d2.setPosition({0, 0});

    std::vector<Disc> discs{d1, d2};

    auto collisions = mathutils::findCollidingDiscs(discs, 5);
    EXPECT_EQ(collisions.size(), 1);

    d2.setPosition({5, 0});
    discs = {d1, d2};

    collisions = mathutils::findCollidingDiscs(discs, 5);
    EXPECT_EQ(collisions.size(), 1);

    auto pair = mathutils::makeOrderedPair(&discs[0], &discs[1]);

    EXPECT_TRUE(collisions.contains(pair));

    d2.setPosition({10, 0});
    discs = {d1, d2};

    collisions = mathutils::findCollidingDiscs(discs, 5);
    EXPECT_EQ(collisions.size(), 0);

    Disc d3(&Mass5);
    d2.setPosition({0, 0});
    d3.setPosition({0, 0});

    discs = {d1, d2, d3};
    collisions = mathutils::findCollidingDiscs(discs, 5);

    // In theory we have: (d1, d2), (d1, d3), (d2, d3)
    // In practice we don't support multiple collisions, so we get just one of the above

    auto pair1 = mathutils::makeOrderedPair(&discs[0], &discs[1]);
    auto pair2 = mathutils::makeOrderedPair(&discs[0], &discs[2]);
    auto pair3 = mathutils::makeOrderedPair(&discs[1], &discs[2]);

    int collisionCount = 0;
    if (collisions.contains(pair1))
        ++collisionCount;
    if (collisions.contains(pair2))
        ++collisionCount;
    if (collisions.contains(pair3))
        ++collisionCount;

    EXPECT_EQ(collisionCount, 1);
}

TEST(MathUtilsTest, CollisionsWithBounds)
{
    const sf::Vector2d boundsTopLeft{0, 0};
    const sf::Vector2d boundsBottomRight{100, 100};

    Disc d(&Mass5);
    const double R = d.getType()->getRadius();

    // Collision with top and right wall
    d.setPosition(boundsTopLeft);
    d.setVelocity({-1.0, 1.0});

    mathutils::handleWorldBoundCollision(d, boundsTopLeft, boundsBottomRight, 0);

    // The disc is already R units behind the wall, so the algorithm should move it back to where it should
    // have collided, calculate the new velocity, calculate how long it took the disc to travel R units, and then move
    // it in the direction of the new velocity for that time
    EXPECT_NEAR(d.getPosition().x, boundsTopLeft.x + 2 * R, 1e-4f);
    EXPECT_NEAR(d.getPosition().y, boundsTopLeft.y + 2 * R, 1e-4f);

    // Collision with bottom and left wall
    d.setPosition(boundsBottomRight);
    d.setVelocity({1.0, -1.0});

    mathutils::handleWorldBoundCollision(d, boundsTopLeft, boundsBottomRight, 0);

    EXPECT_NEAR(d.getPosition().x, boundsBottomRight.x - 2 * R, 1e-4f);
    EXPECT_NEAR(d.getPosition().y, boundsBottomRight.y - 2 * R, 1e-4f);
}

TEST(MathUtilsTest, Abs)
{
    EXPECT_NEAR(mathutils::abs(sf::Vector2d{1, 1}), std::sqrt(2), 1e-4);
    EXPECT_NEAR(mathutils::abs(sf::Vector2d{0, 0}), 0, 0);
}

// Kind of a system test that tests the entire collision response
TEST(MathUtilsTest, CollisionHandling)
{
    DiscType discType("B", sf::Color::Red, 1.0, 1.0);
    Disc d1(&discType), d2(&discType);

    sf::Vector2d d1InitialPosition{0, 0};
    d1.setPosition(d1InitialPosition);
    d1.setVelocity({1.1f, -1});

    // d2 touches d1 at time t = 0, but no collision yet
    const auto sqrt2 = std::numbers::sqrt2;
    sf::Vector2d d2InitialPosition{sqrt2, -sqrt2};
    d2.setPosition(d2InitialPosition);
    d2.setVelocity({1.2f, 1});

    const double kineticEnergyBefore = d1.getKineticEnergy() + d2.getKineticEnergy();

    const double dt = 0.15f;

    d1.move(dt * d1.getVelocity());
    d2.move(dt * d2.getVelocity());

    const auto& overlapResults = mathutils::calculateOverlap(d1, d2);
    const double calculatedDt = mathutils::calculateTimeBeforeCollision(d1, d2, overlapResults);

    EXPECT_DOUBLE_EQ(-dt, calculatedDt);

    mathutils::handleDiscCollisions({std::make_pair(&d1, &d2)});

    const double kineticEnergyAfter = d1.getKineticEnergy() + d2.getKineticEnergy();

    // Values calculated once for regression testing

    expectNear(d1.getPosition(), {0.0225f, -0.0075f}, 1e-4f);
    expectNear(d2.getPosition(), {1.73671f, -1.40671f}, 1e-4f);

    expectNear(d1.getVelocity(), {0.15f, -0.05f}, 1e-4f);
    expectNear(d2.getVelocity(), {2.15f, 0.05f}, 1e-4f);

    // For the small velocities, this should be about equal
    // For higher velocities (in the hundreds), we'd expect a diff of 1 or 2

    EXPECT_DOUBLE_EQ(kineticEnergyBefore, kineticEnergyAfter);
}

TEST(MathUtilsTest, getRandomFloat)
{
    double f1 = mathutils::getRandomFloat();
    double f2 = mathutils::getRandomFloat();

    EXPECT_NE(f1, f2); // Maybe

    EXPECT_GE(f1, 0.0f);
    EXPECT_LE(f1, 1.0f);

    EXPECT_GE(f2, 0.0f);
    EXPECT_LE(f2, 1.0f);
}

TEST(MathUtilsTest, makeOrderedPair)
{
    int i1 = 0, i2 = 0;
    auto pair = mathutils::makeOrderedPair(&i1, &i2);

    std::swap(i1, i2);
    auto pair2 = mathutils::makeOrderedPair(&i1, &i2);

    EXPECT_EQ(pair, pair2);
}
