#include "Reactions.hpp"
#include "Disc.hpp"
#include "DiscType.hpp"
#include "GlobalSettings.hpp"
#include "MathUtils.hpp"
#include "Reaction.hpp"
#include "TestUtils.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>

TEST(ReactionsTest, combinationReaction)
{
    cell::DiscType::map<int> distribution{{&Mass5Radius5, 100}, {&Mass10Radius5, 0}, {&Mass5Radius10, 0}};
    cell::Reaction combination{&Mass5Radius5, &Mass5Radius10, &Mass10Radius5, nullptr, 1};
    cell::GlobalSettings::get().setDiscTypeDistribution(distribution);
    cell::GlobalSettings::get().addReaction(combination);

    cell::Disc d1(&Mass5Radius5), d2(&Mass5Radius10);

    d1.setPosition({2, 2});
    d2.setPosition({2, 2});

    d1.setVelocity({1, 0});
    d2.setVelocity({-1, 0});

    EXPECT_TRUE(combinationReaction(&d1, &d2));
    EXPECT_TRUE(d2.isMarkedDestroyed());
    EXPECT_DOUBLE_EQ(cell::mathutils::abs(d1.getVelocity()), 0);
    EXPECT_EQ(d1.getType(), &Mass10Radius5);
    expectNear(d1.getPosition(), {2.0, 2.0}, 1e-4);
}

TEST(ReactionsTest, decompositionReaction)
{
    // TODO Prevent decomposition if there is not enough space available around the disc
    cell::DiscType::map<int> distribution{{&Mass5Radius5, 100}, {&Mass10Radius5, 0}};
    cell::Reaction decomposition{&Mass10Radius5, nullptr, &Mass5Radius5, &Mass5Radius5, 1};
    cell::GlobalSettings::get().setDiscTypeDistribution(distribution);
    cell::GlobalSettings::get().addReaction(decomposition);

    // Probabilities for decomposition reactions are in %/s, so we need the time step to be
    cell::GlobalSettings::get().setSimulationTimeStep(cell::SettingsLimits::MaxSimulationTimeStep);

    cell::Disc d1(&Mass10Radius5);
    d1.setPosition({10, 10});
    d1.setVelocity({1, 1});

    // Since the reaction has a chance of 100% in 1 second, we need 1/dt tries where dt is the simulation time step in
    // seconds
    double dt = cell::GlobalSettings::getSettings().simulationTimeStep_.asSeconds();
    int N = static_cast<int>(1.0 / dt) + 1;
    std::vector<cell::Disc> newDiscs;

    for (int i = 0; i < N && newDiscs.empty(); ++i)
        decompositionReaction(&d1, newDiscs);

    EXPECT_TRUE(d1.isMarkedDestroyed());
    ASSERT_EQ(newDiscs.size(), 2);

    const auto& v = d1.getVelocity();
    const auto& v1 = newDiscs[0].getVelocity();
    const auto& v2 = newDiscs[1].getVelocity();

    // u * v = |u|*|v|*cos(alpha)
    // We expect the new discs to move away perpendicular to the old velocity, so alpha = 180
    EXPECT_DOUBLE_EQ(v1.x * v2.x + v1.y * v2.y, -cell::mathutils::abs(v1) * cell::mathutils::abs(v2));
    EXPECT_DOUBLE_EQ(v1.x * v.x + v1.y * v.y, 0);
    EXPECT_DOUBLE_EQ(v2.x * v.x + v2.y * v.y, 0);

    EXPECT_EQ(newDiscs[0].getType(), decomposition.getProduct1());
    EXPECT_EQ(newDiscs[1].getType(), decomposition.getProduct2());

    expectNear(newDiscs[0].getPosition(), d1.getPosition(), 1e-4);
    expectNear(newDiscs[1].getPosition(), d1.getPosition(), 1e-4);
}

TEST(ReactionsTest, exchangeReaction)
{
    cell::DiscType::map<int> distribution{{&Mass5Radius5, 100}, {&Mass10Radius5, 0}, {&Mass15Radius10, 0}};
    cell::Reaction exchange{&Mass5Radius5, &Mass15Radius10, &Mass10Radius5, &Mass10Radius5, 1};
    cell::GlobalSettings::get().setDiscTypeDistribution(distribution);
    cell::GlobalSettings::get().addReaction(exchange);

    cell::Disc d1(&Mass5Radius5), d2(&Mass15Radius10);
    d1.setPosition({2, 2});
    d2.setPosition({2, 2});

    d1.setVelocity({1, 1});
    d2.setVelocity({1.2, 1.3});

    double kineticEnergyBefore = d1.getKineticEnergy() + d2.getKineticEnergy();

    ASSERT_TRUE(exchangeReaction(&d1, &d2));

    double kineticEnergyAfter = d1.getKineticEnergy() + d2.getKineticEnergy();

    EXPECT_DOUBLE_EQ(kineticEnergyBefore, kineticEnergyAfter);

    EXPECT_EQ(d1.getType(), exchange.getProduct1());
    EXPECT_EQ(d2.getType(), exchange.getProduct2());

    expectNear(d1.getPosition(), {2.0, 2.0}, 1e-4);
    expectNear(d2.getPosition(), {2.0, 2.0}, 1e-4);

    // TODO: Make sure that total momentum is also conserved (need to fix exchange reaction physics)
}

TEST(ReactionsTest, transformationReaction)
{
    cell::DiscType::map<int> distribution{{&Mass5Radius5, 100}, {&Mass5Radius10, 0}};
    cell::Reaction transformation{&Mass5Radius5, nullptr, &Mass5Radius10, nullptr, 1};
    cell::GlobalSettings::get().setDiscTypeDistribution(distribution);
    cell::GlobalSettings::get().addReaction(transformation);

    cell::GlobalSettings::get().setSimulationTimeStep(cell::SettingsLimits::MaxSimulationTimeStep);

    cell::Disc d1(&Mass5Radius5);
    d1.setPosition({10, 10});
    d1.setVelocity({1, 1});

    double dt = cell::GlobalSettings::getSettings().simulationTimeStep_.asSeconds();
    int N = static_cast<int>(1.0 / dt) + 1;

    for (int i = 0; i < N; ++i)
    {
        if (transformationReaction(&d1))
            break;
    }

    EXPECT_EQ(d1.getType(), transformation.getProduct1());
}

TEST(ReactionTest, transformationProbabilityShouldntIncreaseWithDecreasedSimulationTimeStep)
{
    // Used to be a bug: transformationReaction and decompositionReaction used a static const reference to .asSeconds()
    // which returns a new value and not a reference.

    cell::DiscType::map<int> distribution{{&Mass5Radius5, 100}, {&Mass5Radius10, 0}};
    cell::Reaction transformation{&Mass5Radius5, nullptr, &Mass5Radius10, nullptr, 0.1};
    cell::GlobalSettings::get().setDiscTypeDistribution(distribution);
    cell::GlobalSettings::get().addReaction(transformation);

    std::vector<cell::Disc> discs1(1000, cell::Disc(&Mass5Radius5));
    std::vector<cell::Disc> discs2 = discs1;

    // Big time step

    cell::GlobalSettings::get().setSimulationTimeStep(cell::SettingsLimits::MaxSimulationTimeStep);

    int N = static_cast<int>(sf::seconds(10) / cell::SettingsLimits::MaxSimulationTimeStep);

    for (int i = 0; i < N; ++i)
    {
        for (auto& disc : discs1)
            transformationReaction(&disc);
    }

    // Smaller time step

    auto newTimeStep = 0.1f * cell::SettingsLimits::MaxSimulationTimeStep;
    cell::GlobalSettings::get().setSimulationTimeStep(newTimeStep);

    N = static_cast<int>(sf::seconds(10) / newTimeStep);

    for (int i = 0; i < N; ++i)
    {
        for (auto& disc : discs2)
            transformationReaction(&disc);
    }

    auto transformed1 = std::count_if(discs1.begin(), discs1.end(),
                                      [](const cell::Disc& disc) { return disc.getType() == &Mass5Radius10; });
    auto transformed2 = std::count_if(discs2.begin(), discs2.end(),
                                      [](const cell::Disc& disc) { return disc.getType() == &Mass5Radius10; });

    EXPECT_LT(std::abs(transformed1 - transformed2), 100);
}