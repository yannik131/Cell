#include <gtest/gtest.h>

#include "GlobalSettings.hpp"

#include <cmath>

TEST(GlobalSettingsTest, RangeChecksAreCorrect)
{
    GlobalSettings& settings = GlobalSettings::get();

    using namespace SettingsLimits;

    ASSERT_ANY_THROW(
        settings.setFrictionCoefficient(std::nextafterf(MinFrictionCoefficient, MinFrictionCoefficient - 1)));
    ASSERT_ANY_THROW(
        settings.setFrictionCoefficient(std::nextafterf(MaxFrictionCoefficient, MaxFrictionCoefficient + 1)));
    ASSERT_NO_THROW(settings.setFrictionCoefficient(MinFrictionCoefficient));
    ASSERT_NO_THROW(settings.setFrictionCoefficient(MaxFrictionCoefficient));

    int minTimeMs = MinSimulationTimeStep.asMilliseconds();
    int maxTimeMs = MaxSimulationTimeStep.asMilliseconds();
    ASSERT_ANY_THROW(settings.setSimulationTimeStep(sf::milliseconds(minTimeMs - 1)));
    ASSERT_ANY_THROW(settings.setSimulationTimeStep(sf::milliseconds(maxTimeMs + 1)));
    ASSERT_NO_THROW(settings.setSimulationTimeStep(sf::milliseconds(minTimeMs)));
    ASSERT_NO_THROW(settings.setSimulationTimeStep(sf::milliseconds(maxTimeMs)));

    ASSERT_ANY_THROW(
        settings.setSimulationTimeScale(std::nextafterf(MinSimulationTimeScale, MinSimulationTimeScale - 1)));
    ASSERT_ANY_THROW(
        settings.setSimulationTimeScale(std::nextafterf(MaxSimulationTimeScale, MaxSimulationTimeScale + 1)));
    ASSERT_NO_THROW(settings.setSimulationTimeScale(MinSimulationTimeScale));
    ASSERT_NO_THROW(settings.setSimulationTimeScale(MaxSimulationTimeScale));

    ASSERT_ANY_THROW(settings.setNumberOfDiscs(MinNumberOfDiscs - 1));
    ASSERT_ANY_THROW(settings.setNumberOfDiscs(MaxNumberOfDiscs + 1));
    ASSERT_NO_THROW(settings.setNumberOfDiscs(MinNumberOfDiscs));
    ASSERT_NO_THROW(settings.setNumberOfDiscs(MaxNumberOfDiscs));
}

TEST(GlobalSettingsTest, CallbackIsExecuted)
{
    SettingID id = SettingID::DiscTypeDistribution;
    const auto& callback = [&](const SettingID& settingID) { id = settingID; };
    GlobalSettings& settings = GlobalSettings::get();
    settings.setCallback(callback);

    settings.setFrictionCoefficient(SettingsLimits::MinFrictionCoefficient);

    ASSERT_EQ(id, SettingID::FrictionCoefficient);
}

TEST(GlobalSettingsTest, LockPreventsChanges)
{
    GlobalSettings& settings = GlobalSettings::get();

    settings.lock();
    settings.unlock();

    // Unlocked: all modification calls succeed.
    ASSERT_NO_THROW(settings.setSimulationTimeStep(SettingsLimits::MinSimulationTimeStep));
    ASSERT_NO_THROW(settings.setSimulationTimeScale(SettingsLimits::MinSimulationTimeScale));
    ASSERT_NO_THROW(settings.setNumberOfDiscs(SettingsLimits::MinNumberOfDiscs));
    ASSERT_NO_THROW(settings.setFrictionCoefficient(SettingsLimits::MinFrictionCoefficient));

    const DiscType discType{"A", sf::Color::Green, DiscTypeLimits::MinRadius, DiscTypeLimits::MinMass};
    const std::map<DiscType, int> distribution{{discType, 100}};
    ASSERT_NO_THROW(settings.setDiscTypeDistribution(distribution));

    const Reaction reaction{discType, discType, discType, std::nullopt, 0.01f};
    ASSERT_NO_THROW(settings.addReaction(reaction));
    ASSERT_NO_THROW(settings.clearReactions());

    // Lock the settings so that further changes are prohibited.
    settings.lock();

    // Now all modification calls should throw ChangeDuringLockException.
    ASSERT_THROW(settings.setSimulationTimeStep(SettingsLimits::MinSimulationTimeStep), ChangeDuringLockException);
    ASSERT_THROW(settings.setSimulationTimeScale(SettingsLimits::MinSimulationTimeScale), ChangeDuringLockException);
    ASSERT_THROW(settings.setNumberOfDiscs(SettingsLimits::MinNumberOfDiscs), ChangeDuringLockException);
    ASSERT_THROW(settings.setFrictionCoefficient(SettingsLimits::MinFrictionCoefficient), ChangeDuringLockException);
    ASSERT_THROW(settings.setDiscTypeDistribution(distribution), ChangeDuringLockException);
    ASSERT_THROW(settings.addReaction(reaction), ChangeDuringLockException);
    ASSERT_THROW(settings.clearReactions(), ChangeDuringLockException);

    settings.unlock();
}

TEST(GlobalSettingsTest, IsLockedReturnsCorrectValues)
{
    GlobalSettings& settings = GlobalSettings::get();

    ASSERT_FALSE(settings.isLocked());

    settings.lock();
    ASSERT_TRUE(settings.isLocked());

    settings.unlock();
    ASSERT_FALSE(settings.isLocked());
}