#include "Cell.hpp"
#include "GlobalSettings.hpp"

#include <SFML/System/Time.hpp>
#include <glog/logging.h>
#include <gtest/gtest.h>

TEST(WorldTest, EnergyIsConserved)
{
    GlobalSettings::get().restoreDefault();

    Cell world;
    world.setBounds(sf::Vector2f{500, 500});
    world.reinitialize();

    float initialKineticEnergy = world.getInitialKineticEnergy();
    int totalCollisionCount = 0;
    int collisionTarget = 100;
    int updateCount = 0;

    while (totalCollisionCount < collisionTarget)
    {
        world.update(sf::milliseconds(1));

        for (const auto& [discType, collisionCount] : world.getAndResetCollisionCount())
            totalCollisionCount += collisionCount;

        if (++updateCount > collisionTarget * 10)
            FAIL() << "There is just no way we don't have at least 1 collision per 10 updates";
    }

    DLOG(INFO) << "Took " << updateCount << " updates to reach " << collisionTarget << " collisions ("
               << collisionTarget / static_cast<float>(updateCount) << " collisions/s)";

    // Currently, combination reactions don't conserve kinetic energy
    EXPECT_NEAR(world.getCurrentKineticEnergy(), initialKineticEnergy, 0.01f * initialKineticEnergy);
}