#include "Cell.hpp"
#include "GlobalSettings.hpp"

#include <SFML/System/Time.hpp>
#include <glog/logging.h>
#include <gtest/gtest.h>

TEST(CellTest, EnergyIsConserved)
{
    cell::GlobalSettings::get().restoreDefault();

    cell::Cell cell;
    cell.setBounds(sf::Vector2f{500, 500});
    cell.reinitialize();

    float initialKineticEnergy = cell.getInitialKineticEnergy();
    int totalCollisionCount = 0;
    int collisionTarget = 100;
    int updateCount = 0;

    while (totalCollisionCount < collisionTarget)
    {
        cell.update(sf::milliseconds(1));

        for (const auto& [discType, collisionCount] : cell.getAndResetCollisionCount())
            totalCollisionCount += collisionCount;

        if (++updateCount > collisionTarget * 10)
            FAIL() << "There is just no way we don't have at least 1 collision per 10 updates";
    }

    DLOG(INFO) << "Took " << updateCount << " updates to reach " << collisionTarget << " collisions ("
               << static_cast<float>(collisionTarget) / static_cast<float>(updateCount) << " collisions/s)";

    // Currently, combination reactions don't conserve kinetic energy
    EXPECT_NEAR(cell.getCurrentKineticEnergy(), initialKineticEnergy, 0.02f * initialKineticEnergy);
}