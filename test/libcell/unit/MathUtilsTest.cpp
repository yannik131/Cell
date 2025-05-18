#include "MathUtils.hpp"

#include <glog/logging.h>
#include <gtest/gtest.h>

static const DiscType t("A", sf::Color::Green, 5.f, 5.f);

TEST(MathUtilsTest, OperatorPlusEqualsWorksForMaps)
{
    std::map<int, double> m1{{1, 1.0}, {2, 2.0}};
    std::map<int, double> m2{{1, 2.0}, {2, 3.0}, {3, 3.0}};

    m1 += m2;

    EXPECT_DOUBLE_EQ(m1[1], 3.0);
    EXPECT_DOUBLE_EQ(m1[2], 5.0);
}

TEST(MathUtilsTest, OperatorDivideEqualsWorksForMaps)
{
    std::map<int, double> m1{{1, 1.0}, {2, 2.0}};
    m1 /= 2.0;

    EXPECT_DOUBLE_EQ(m1[1], 0.5);
    EXPECT_DOUBLE_EQ(m1[2], 1.0);
}

TEST(MathUtilsTest, FindsCollidingDiscs)
{
    Disc d1(t), d2(t);
    d1.setPosition({0, 0});
    d2.setPosition({0, 0});

    std::vector<Disc> discs{d1, d2};

    auto collisions = MathUtils::findCollidingDiscs(discs, 5);
    EXPECT_EQ(collisions.size(), 1);

    d2.setPosition({5, 0});
    discs = {d1, d2};

    collisions = MathUtils::findCollidingDiscs(discs, 5);
    EXPECT_EQ(collisions.size(), 1);

    auto pair = MathUtils::makeOrderedPair(&discs[0], &discs[1]);

    EXPECT_TRUE(collisions.contains(pair));

    d2.setPosition({10, 0});
    discs = {d1, d2};

    collisions = MathUtils::findCollidingDiscs(discs, 5);
    EXPECT_EQ(collisions.size(), 0);

    Disc d3(t);
    d2.setPosition({0, 0});
    d3.setPosition({0, 0});

    discs = {d1, d2, d3};
    collisions = MathUtils::findCollidingDiscs(discs, 5);

    // In theory we have: (d1, d2), (d1, d3), (d2, d3)
    // In practice we don't support multiple collisions, so we get just one of the above

    auto pair1 = MathUtils::makeOrderedPair(&discs[0], &discs[1]);
    auto pair2 = MathUtils::makeOrderedPair(&discs[0], &discs[2]);
    auto pair3 = MathUtils::makeOrderedPair(&discs[1], &discs[2]);

    int collisionCount = 0;
    if (collisions.contains(pair1))
        ++collisionCount;
    if (collisions.contains(pair2))
        ++collisionCount;
    if (collisions.contains(pair3))
        ++collisionCount;

    EXPECT_EQ(collisionCount, 1);
}

TEST(MathutilsTest, HandleDiscCollisions)
{
    FAIL();
}

TEST(MathUtilsTest, DecompositionReaction)
{
    FAIL();
}

TEST(MathUtilsTest, CollisionsWithBounds)
{
    const sf::Vector2f boundsTopLeft{0, 0};
    const sf::Vector2f boundsBottomRight{100, 100};

    Disc d(t);
    const float R = d.getType().getRadius();

    // Collision with top and right wall
    d.setPosition(boundsTopLeft);
    d.setVelocity({-1.f, 1.f});

    MathUtils::handleWorldBoundCollision(d, boundsTopLeft, boundsBottomRight, 0);

    // The disc is already R units behind the wall, so the algorithm should move it back to where it should
    // have collided, calculate the new velocity, calculate how long it took the disc to travel R units, and then move
    // it in the direction of the new velocity for that time
    EXPECT_NEAR(d.getPosition().x, boundsTopLeft.x + 2 * R, 1e-4f);
    EXPECT_NEAR(d.getPosition().y, boundsTopLeft.y + 2 * R, 1e-4f);

    // Collision with bottom and left wall
    d.setPosition(boundsBottomRight);
    d.setVelocity({1.f, -1.f});

    MathUtils::handleWorldBoundCollision(d, boundsTopLeft, boundsBottomRight, 0);

    EXPECT_NEAR(d.getPosition().x, boundsBottomRight.x - 2 * R, 1e-4f);
    EXPECT_NEAR(d.getPosition().y, boundsBottomRight.y - 2 * R, 1e-4f);
}

TEST(MathUtilsTest, Abs)
{
    EXPECT_NEAR(MathUtils::abs(sf::Vector2f{1, 1}), std::sqrt(2), 1e-4);
    EXPECT_NEAR(MathUtils::abs(sf::Vector2f{0, 0}), 0, 0));
}

TEST(MathUtilsTest, CalculateOverlap)
{
    FAIL();
}

TEST(MathUtilsTest, TimeOfCollisionIsCalculatedCorrectly)
{
    DiscType discType("B", sf::Color::Red, 1.f, 1.f);
    Disc d1(discType), d2(discType);

    d1.setPosition({0, 0});
    d1.setVelocity({1, -1});

    // d2 touches d1 at time t = 0, but no collision yet
    const float sqrt2 = std::sqrt(2);
    d2.setPosition({sqrt2, -sqrt2});
    d2.setVelocity({1, 1});

    const float dt = 0.15f;

    d1.move(dt * d1.getVelocity());
    d2.move(dt * d2.getVelocity());

    const auto& overlapResults = MathUtils::calculateOverlap(d1, d2);
    const float calculatedDt = MathUtils::calculateTimeBeforeCollision(d1, d2, overlapResults);

    EXPECT_NEAR(-dt, calculatedDt, 1e-4f);
}

TEST(MathUtilsTest, UpdateVelocitiesAtCollision)
{
    FAIL();
}

TEST(MathUtilsTest, InvertMap)
{
    FAIL();
}

TEST(MathUtilsTest, getRandomFloat)
{
    FAIL();
}

TEST(MathUtilsTest, makeOrderedPair)
{
    FAIL();
}
