#include "cell/CollisionDetector.hpp"
#include "TestUtils.hpp"
#include "cell/Disc.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace cell;
using namespace testing;

class ACollisionDetector : public Test
{
protected:
    DiscTypeRegistry registry;
    DiscTypeID A = {};
    std::unique_ptr<CollisionDetector> collisionDetector;

    void SetUp() override
    {
        std::vector<DiscType> types;
        types.emplace_back("A", Radius{5}, Mass{5});

        registry.setValues(std::move(types));
        collisionDetector = std::make_unique<CollisionDetector>(CollisionDetector(registry));

        A = registry.getIDFor("A");
    }
};

std::vector<std::pair<Disc*, Disc*>> createPossiblePairs(std::vector<Disc>& discs)
{
    std::vector<std::pair<Disc*, Disc*>> pairs;
    for (std::size_t i = 0; i < discs.size(); ++i)
    {
        for (std::size_t j = i; j < discs.size(); ++j)
        {
            pairs.emplace_back(&discs[i], &discs[j]);
            if (i != j)
                pairs.emplace_back(&discs[j], &discs[i]);
        }
    }

    return pairs;
}

TEST_F(ACollisionDetector, FindsCollidingDiscs)
{
    Disc d1(A), d2(A), d3(A);
    std::vector<Disc> discs = {d1, d2, d3};
    auto collisions = collisionDetector->detectDiscDiscCollisions(discs);

    // In theory we have: (d1, d2), (d1, d3), (d2, d3) (or swapped versions of these)
    // In practice we don't support multiple collisions, so we get just one of the above

    int count = 0;
    for (const auto& pair : createPossiblePairs(discs))
    {
        if (std::find(collisions.begin(), collisions.end(), pair) != collisions.end())
            ++count;
    }

    ASSERT_THAT(count, Eq(1));
}

TEST_F(ACollisionDetector, IgnoreDiscsThatArentColliding)
{
    Disc d1(A), d2(A);
    d1.setPosition({100, 100});
    std::vector<Disc> discs = {d1, d2};

    auto collisions = collisionDetector->detectDiscDiscCollisions(discs);

    ASSERT_THAT(collisions.empty(), Eq(true));
}