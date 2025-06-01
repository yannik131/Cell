#include "World.hpp"

#include <SFML/System/Time.hpp>
#include <gtest/gtest.h>

TEST(WorldTest, EnergyIsConserved)
{
    World world;
    world.setBounds(sf::Vector2f{500, 500});
    world.reinitialize();

    float initialKineticEnergy = world.getInitialKineticEnergy();

    for (int i = 0; i < 5; ++i)
        world.update(sf::milliseconds(1));

    EXPECT_NEAR(world.getCurrentKineticEnergy(), initialKineticEnergy, 0.01f * initialKineticEnergy);
}