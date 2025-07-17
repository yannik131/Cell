#include "CollisionDetector.hpp"
#include "Disc.hpp"
#include "GlobalSettings.hpp"
#include "MathUtils.hpp"
#include "TestUtils.hpp"

#include <gtest/gtest.h>

TEST(CollisionDetectorTest, CanDetectDiscDiscCollisions)
{
    cell::GlobalSettings::get().setDiscTypeDistribution({{Mass5, 100}});

    cell::Disc d1(Mass5), d2(Mass5);
    d1.setPosition({0, 0});
    d2.setPosition({0, 0});

    std::vector<cell::Disc> discs{d1, d2};
    cell::CollisionDetector collisionDetector;

    std::vector<cell::Membrane> membranes;
    collisionDetector.detectCollisions(discs, membranes);
    auto collisions = collisionDetector.getDiscDiscCollisions();

    EXPECT_EQ(collisions.size(), 1);

    d2.setPosition({5, 0});
    discs = {d1, d2};

    collisionDetector.detectCollisions(discs, membranes);
    collisions = collisionDetector.getDiscDiscCollisions();
    EXPECT_EQ(collisions.size(), 1);

    auto pair = cell::mathutils::makeOrderedPair(&discs[0], &discs[1]);

    EXPECT_TRUE(collisions.contains(pair));

    d2.setPosition({10, 0});
    discs = {d1, d2};

    collisionDetector.detectCollisions(discs, membranes);
    collisions = collisionDetector.getDiscDiscCollisions();
    EXPECT_EQ(collisions.size(), 0);

    cell::Disc d3(Mass5);
    d2.setPosition({0, 0});
    d3.setPosition({0, 0});

    discs = {d1, d2, d3};
    collisionDetector.detectCollisions(discs, membranes);
    collisions = collisionDetector.getDiscDiscCollisions();

    // In theory we have: (d1, d2), (d1, d3), (d2, d3)
    // In practice we don't support multiple collisions, so we get just one of the above

    auto pair1 = cell::mathutils::makeOrderedPair(&discs[0], &discs[1]);
    auto pair2 = cell::mathutils::makeOrderedPair(&discs[0], &discs[2]);
    auto pair3 = cell::mathutils::makeOrderedPair(&discs[1], &discs[2]);

    int collisionCount = 0;
    if (collisions.contains(pair1))
        ++collisionCount;
    if (collisions.contains(pair2))
        ++collisionCount;
    if (collisions.contains(pair3))
        ++collisionCount;

    EXPECT_EQ(collisionCount, 1);
}