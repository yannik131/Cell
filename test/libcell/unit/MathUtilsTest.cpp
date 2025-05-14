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

std::pair<Disc*, Disc*> orderedPointers(Disc& d1, Disc& d2)
{
    auto pair = std::make_pair(&d1, &d2);
    if (&d1 > &d2)
        std::swap(pair.first, pair.second);

    return pair;
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

    auto pair = orderedPointers(discs[0], discs[1]);

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
    // In practice we don't support multiple collisions, so we get ((d1, d2) or (d1, d3)) and (d2, d3)
    EXPECT_EQ(collisions.size(), 2);

    auto pair1 = orderedPointers(discs[0], discs[1]);
    auto pair2 = orderedPointers(discs[0], discs[2]);
    auto pair3 = orderedPointers(discs[1], discs[2]);

    EXPECT_TRUE(collisions.contains(pair1) || collisions.contains(pair2));
    EXPECT_TRUE(collisions.contains(pair3));
}

TEST(MathUtilsTest, CorrectOverlap)
{
    DiscType discType("B", sf::Color::Red, 1.f, 1.f);
    Disc d1(discType), d2(discType);

    d1.setPosition({0, 0});
    d1.setVelocity({1, -1});

    const float sqrt2 = std::sqrt(2);
    d2.setPosition({sqrt2, -sqrt2});
    d2.setVelocity({1, 1});

    d1.move(0.1f * d1.getVelocity());
    d2.move(0.1f * d2.getVelocity());

    const auto& [normal, distance, overlap] = MathUtils::correctOverlap(d1, d2);

    EXPECT_NEAR(d1.getPosition().x, 0, 1e-4);
    EXPECT_NEAR(d1.getPosition().y, 0, 1e-4);

    EXPECT_NEAR(d2.getPosition().x, sqrt2, 1e-4);
    EXPECT_NEAR(d2.getPosition().y, -sqrt2, 1e-4);
}
