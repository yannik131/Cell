#include "MathUtils.hpp"
#include "TestUtils.hpp"

#include <glog/logging.h>
#include <gtest/gtest.h>

#include <cmath>
#include <unordered_set>

static const DiscType t("A", sf::Color::Green, 5.f, 5.f);

TEST(MathUtilsTest, OperatorPlusEqualsWorksForMaps)
{
    std::unordered_map<int, double> m1{{1, 1.0}, {2, 2.0}};
    std::unordered_map<int, double> m2{{1, 2.0}, {2, 3.0}, {3, 3.0}};

    m1 += m2;

    EXPECT_DOUBLE_EQ(m1[1], 3.0);
    EXPECT_DOUBLE_EQ(m1[2], 5.0);
}

TEST(MathUtilsTest, OperatorDivideEqualsWorksForMaps)
{
    std::unordered_map<int, double> m1{{1, 1.0}, {2, 2.0}};
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
    EXPECT_NEAR(MathUtils::abs(sf::Vector2f{0, 0}), 0, 0);
}

// Kind of a system test that tests the entire collision response
TEST(MathUtilsTest, CollisionHandling)
{
    DiscType discType("B", sf::Color::Red, 1.f, 1.f);
    Disc d1(discType), d2(discType);

    sf::Vector2f d1InitialPosition{0, 0};
    d1.setPosition(d1InitialPosition);
    d1.setVelocity({1.1f, -1});

    // d2 touches d1 at time t = 0, but no collision yet
    const float sqrt2 = std::sqrt(2);
    sf::Vector2f d2InitialPosition{sqrt2, -sqrt2};
    d2.setPosition(d2InitialPosition);
    d2.setVelocity({1.2f, 1});

    float kineticEnergyBefore = d1.getKineticEnergy() + d2.getKineticEnergy();

    const float dt = 0.15f;

    d1.move(dt * d1.getVelocity());
    d2.move(dt * d2.getVelocity());

    const auto& overlapResults = MathUtils::calculateOverlap(d1, d2);
    const float calculatedDt = MathUtils::calculateTimeBeforeCollision(d1, d2, overlapResults);

    EXPECT_FLOAT_EQ(-dt, calculatedDt);

    MathUtils::handleDiscCollisions({std::make_pair(&d1, &d2)});

    float kineticEnergyAfter = d1.getKineticEnergy() + d2.getKineticEnergy();

    // Values calculated once for regression testing

    expectNear(d1.getPosition(), {0.0225f, -0.0075f}, 1e-4f);
    expectNear(d2.getPosition(), {1.73671f, -1.40671f}, 1e-4f);

    expectNear(d1.getVelocity(), {0.15f, -0.05f}, 1e-4f);
    expectNear(d2.getVelocity(), {2.15f, 0.05f}, 1e-4f);

    EXPECT_FLOAT_EQ(kineticEnergyBefore, kineticEnergyAfter);
}

TEST(MathUtilsTest, getRandomFloat)
{
    float f1 = MathUtils::getRandomFloat();
    float f2 = MathUtils::getRandomFloat();

    EXPECT_NE(f1, f2); // Maybe

    EXPECT_GE(f1, 0.0f);
    EXPECT_LE(f1, 1.0f);

    EXPECT_GE(f2, 0.0f);
    EXPECT_LE(f2, 1.0f);
}

TEST(MathUtilsTest, makeOrderedPair)
{
    int i1, i2;
    auto pair = MathUtils::makeOrderedPair(&i1, &i2);

    std::swap(i1, i2);
    auto pair2 = MathUtils::makeOrderedPair(&i1, &i2);

    EXPECT_EQ(pair, pair2);
}

TEST(MathUtilsTest, calculateHash)
{
    std::unordered_set<int> hashSet;

    for (int x = 0; x <= 100; ++x)
    {
        for (int y = 0; y <= 100; ++y)
        {
            int hashValue = MathUtils::calculateHash(x, y);
            if (hashSet.find(hashValue) != hashSet.end())
                FAIL() << "Duplicate hash value for " << x << " " << y;

            hashSet.insert(hashValue);
        }
    }
}