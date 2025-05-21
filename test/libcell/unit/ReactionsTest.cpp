#include "Reactions.hpp"
#include "Disc.hpp"
#include "DiscType.hpp"
#include "GlobalSettings.hpp"
#include "MathUtils.hpp"
#include "Reaction.hpp"

#include <gtest/gtest.h>

#include <cmath>

namespace
{

DiscType discType(float radius, float mass)
{
    static int count = 0;

    char name = static_cast<int>('A') + count++ % 20;
    return DiscType{std::string(1, name), sf::Color::Green, radius, mass};
}

const DiscType Mass5Radius5 = discType(5, 5);
const DiscType Mass10Radius5 = discType(5, 10);
const DiscType Mass5Radius10 = discType(10, 5);
const DiscType Mass15Radius10 = discType(10, 15);

} // namespace

TEST(ReactionsTest, combinationReaction)
{
    DiscType::map<int> distribution{{Mass5Radius5, 100}, {Mass10Radius5, 0}, {Mass5Radius10, 0}};
    Reaction combination{Mass5Radius5, Mass5Radius10, Mass10Radius5, std::nullopt, 1};
    GlobalSettings::get().setDiscTypeDistribution(distribution);
    GlobalSettings::get().addReaction(combination);

    Disc d1(Mass5Radius5), d2(Mass5Radius10);

    d1.setPosition({0, 0});
    d2.setPosition({0, 0});

    d1.setVelocity({1, 0});
    d2.setVelocity({-1, 0});

    EXPECT_TRUE(combinationReaction(&d1, &d2));
    EXPECT_TRUE(d2.isMarkedDestroyed());
    EXPECT_FLOAT_EQ(MathUtils::abs(d1.getVelocity()), 0);
    EXPECT_EQ(d1.getType(), Mass10Radius5);
}

TEST(ReactionsTest, decompositionReaction)
{
    DiscType::map<int> distribution{{Mass5Radius5, 100}, {Mass10Radius5, 0}};
    Reaction decomposition{Mass10Radius5, std::nullopt, Mass5Radius5, Mass5Radius5, 1};
    GlobalSettings::get().setDiscTypeDistribution(distribution);
    GlobalSettings::get().addReaction(decomposition);

    // Probabilities for decomposition reactions are in %/s, so we need the time step to be
    GlobalSettings::get().setSimulationTimeStep(SettingsLimits::MaxSimulationTimeStep);

    Disc d1(Mass10Radius5);
    d1.setPosition({0, 0});
    d1.setVelocity({1, 1});

    // Since the reaction has a chance of 100% in 1 second, we need 1/dt tries where dt is the simulation time step in
    // seconds
    float dt = GlobalSettings::getSettings().simulationTimeStep_.asSeconds();
    int N = static_cast<int>(1.f / dt) + 1;
    std::vector<Disc> newDiscs;

    for (int i = 0; i < N && newDiscs.empty(); ++i)
        decompositionReaction(&d1, newDiscs);

    EXPECT_TRUE(d1.isMarkedDestroyed());
    ASSERT_EQ(newDiscs.size(), 2);

    const auto& v = d1.getVelocity();
    const auto& v1 = newDiscs[0].getVelocity();
    const auto& v2 = newDiscs[1].getVelocity();

    // u * v = |u|*|v|*cos(alpha)
    // We expect the new discs to move away perpendicular to the old velocity, so alpha = 180
    EXPECT_FLOAT_EQ(v1.x * v2.x + v1.y * v2.y, -MathUtils::abs(v1) * MathUtils::abs(v2));
    EXPECT_FLOAT_EQ(v1.x * v.x + v1.y * v.y, 0);
    EXPECT_FLOAT_EQ(v2.x * v.x + v2.y * v.y, 0);

    EXPECT_EQ(newDiscs[0].getType(), decomposition.getProduct1());
    EXPECT_EQ(newDiscs[1].getType(), decomposition.getProduct2());
}

TEST(ReactionsTest, exchangeReaction)
{
    DiscType::map<int> distribution{{Mass5Radius5, 100}, {Mass10Radius5, 0}, {Mass15Radius10, 0}};
    Reaction exchange{Mass5Radius5, Mass15Radius10, Mass10Radius5, Mass10Radius5, 1};
    GlobalSettings::get().setDiscTypeDistribution(distribution);
    GlobalSettings::get().addReaction(exchange);

    Disc d1(Mass5Radius5), d2(Mass15Radius10);
    d1.setPosition({0, 0});
    d2.setPosition({0, 0});

    d1.setVelocity({1, 1});
    d2.setVelocity({1.2f, 1.3f});

    float kineticEnergyBefore = d1.getKineticEnergy() + d2.getKineticEnergy();
    float momentumBefore = d1.getAbsoluteMomentum() + d2.getAbsoluteMomentum();

    ASSERT_TRUE(exchangeReaction(&d1, &d2));

    float kineticEnergyAfter = d1.getKineticEnergy() + d2.getKineticEnergy();
    float momentumAfter = d1.getAbsoluteMomentum() + d2.getAbsoluteMomentum();

    EXPECT_FLOAT_EQ(kineticEnergyBefore, kineticEnergyAfter);
    // TODO EXPECT_FLOAT_EQ(momentumBefore, momentumAfter);

    EXPECT_EQ(d1.getType(), exchange.getProduct1());
    EXPECT_EQ(d2.getType(), exchange.getProduct2());
}