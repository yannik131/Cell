#include "CollisionHandler.hpp"
#include "Disc.hpp"
#include "DiscTypeRegistry.hpp"
#include "TestUtils.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <numbers>

using namespace cell;
using namespace testing;

class ACollisionHandler : public Test
{
protected:
    DiscTypeRegistry registry;
    DiscTypeResolver resolver;
    DiscTypeID A;
    std::unique_ptr<CollisionHandler> collisionHandler;

    void SetUp() override
    {
        std::vector<DiscType> types;
        types.emplace_back("A", Radius{1}, Mass{1});

        registry.setDiscTypes(std::move(types));
        resolver = registry.getDiscTypeResolver();
        collisionHandler = std::make_unique<CollisionHandler>(CollisionHandler(resolver));

        A = registry.getIDFor("A");
    }
};

TEST_F(ACollisionHandler, SeparatesCollidingDiscs)
{
    const auto sqrt2 = std::numbers::sqrt2;
    const sf::Vector2d d1InitialPosition{0, 0}, d2InitialPosition{sqrt2, -sqrt2};

    Disc d1(A), d2(A);
    d1.setPosition(d1InitialPosition);
    d1.setVelocity({1.1, -1});

    d2.setPosition(d2InitialPosition);
    d2.setVelocity({1.2, 1});

    const double dt = 0.15;
    const double kineticEnergyBefore = d1.getKineticEnergy(resolver) + d2.getKineticEnergy(resolver);

    d1.move(dt * d1.getVelocity());
    d2.move(dt * d2.getVelocity());

    std::set<std::pair<Disc*, Disc*>> collision({std::make_pair(&d1, &d2)});
    collisionHandler->calculateDiscDiscCollisionResponse(collision);

    const double kineticEnergyAfter = d1.getKineticEnergy(resolver) + d2.getKineticEnergy(resolver);

    ASSERT_THAT(kineticEnergyBefore, DoubleNear(kineticEnergyAfter, 1e-4));

    // Just here for regression testing
    expectNear(d1.getPosition(), {0.0225, -0.0075}, 1e-4);
    expectNear(d2.getPosition(), {1.73671, -1.40671}, 1e-4);

    expectNear(d1.getVelocity(), {0.15, -0.05}, 1e-4);
    expectNear(d2.getVelocity(), {2.15, 0.05}, 1e-4);
}

TEST_F(ACollisionHandler, SeparatesDiscsFromBounds)
{
    const sf::Vector2d boundsTopLeft{0, 0};
    const sf::Vector2d boundsBottomRight{100, 100};

    Disc d(A);
    const double R = resolver(A).getRadius();

    d.setPosition(boundsTopLeft);
    d.setVelocity({-1.0, 1.0});

    CollisionDetector collisionDetector(resolver, []() { return 0; });
    auto collision = collisionDetector.detectDiscRectangleCollision(d, boundsTopLeft, boundsBottomRight);

    collisionHandler->calculateDiscRectangleCollisionResponse(d, collision);

    // The disc is already R units behind the wall, so the algorithm should move it back to where it should
    // have collided, calculate the new velocity, calculate how long it took the disc to travel R units, and then move
    // it in the direction of the new velocity for that time
    EXPECT_NEAR(d.getPosition().x, boundsTopLeft.x + 2 * R, 1e-4);
    EXPECT_NEAR(d.getPosition().y, boundsTopLeft.y + 2 * R, 1e-4);

    // Collision with bottom and left wall
    d.setPosition(boundsBottomRight);
    d.setVelocity({1.0, -1.0});

    collision = collisionDetector.detectDiscRectangleCollision(d, boundsTopLeft, boundsBottomRight);

    collisionHandler->calculateDiscRectangleCollisionResponse(d, collision);

    EXPECT_NEAR(d.getPosition().x, boundsBottomRight.x - 2 * R, 1e-4f);
    EXPECT_NEAR(d.getPosition().y, boundsBottomRight.y - 2 * R, 1e-4f);
}