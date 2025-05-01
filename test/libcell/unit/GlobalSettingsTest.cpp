#include <gtest/gtest.h>

#include "GlobalSettings.hpp"

#include <cmath>

const DiscType A{"A", sf::Color::Green, DiscTypeLimits::MinRadius, DiscTypeLimits::MinMass};
const DiscType ACopy{"A", sf::Color::Green, DiscTypeLimits::MinRadius,
                     DiscTypeLimits::MinMass}; // Same attributes, different ID
const DiscType B{"B", sf::Color::Green, DiscTypeLimits::MinRadius, DiscTypeLimits::MinMass};
const DiscType C{"C", sf::Color::Green, DiscTypeLimits::MinRadius, DiscTypeLimits::MinMass};

const Reaction decomposition{A, std::nullopt, A, A, 0.1f};
const Reaction combination{A, A, A, std::nullopt, 0.1f};
const Reaction exchange{A, A, A, A, 0.1f};

GlobalSettings& settings = GlobalSettings::get();

TEST(GlobalSettingsTest, RangeChecksAreCorrect)
{
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
    settings.setCallback(callback);

    settings.setFrictionCoefficient(SettingsLimits::MinFrictionCoefficient);

    ASSERT_EQ(id, SettingID::FrictionCoefficient);
}

TEST(GlobalSettingsTest, LockPreventsChanges)
{
    settings.lock();
    settings.unlock();

    // Unlocked: all modification calls succeed.
    ASSERT_NO_THROW(settings.setSimulationTimeStep(SettingsLimits::MinSimulationTimeStep));
    ASSERT_NO_THROW(settings.setSimulationTimeScale(SettingsLimits::MinSimulationTimeScale));
    ASSERT_NO_THROW(settings.setNumberOfDiscs(SettingsLimits::MinNumberOfDiscs));
    ASSERT_NO_THROW(settings.setFrictionCoefficient(SettingsLimits::MinFrictionCoefficient));

    const std::map<DiscType, int> distribution{{A, 100}};
    ASSERT_NO_THROW(settings.setDiscTypeDistribution(distribution));

    ASSERT_NO_THROW(settings.addReaction(decomposition));
    ASSERT_NO_THROW(settings.clearReactions());

    // Lock the settings so that further changes are prohibited.
    settings.lock();

    // Now all modification calls should throw ChangeDuringLockException.
    ASSERT_THROW(settings.setSimulationTimeStep(SettingsLimits::MinSimulationTimeStep), ChangeDuringLockException);
    ASSERT_THROW(settings.setSimulationTimeScale(SettingsLimits::MinSimulationTimeScale), ChangeDuringLockException);
    ASSERT_THROW(settings.setNumberOfDiscs(SettingsLimits::MinNumberOfDiscs), ChangeDuringLockException);
    ASSERT_THROW(settings.setFrictionCoefficient(SettingsLimits::MinFrictionCoefficient), ChangeDuringLockException);
    ASSERT_THROW(settings.setDiscTypeDistribution(distribution), ChangeDuringLockException);
    ASSERT_THROW(settings.addReaction(decomposition), ChangeDuringLockException);
    ASSERT_THROW(settings.clearReactions(), ChangeDuringLockException);

    settings.unlock();
}

TEST(GlobalSettingsTest, IsLockedReturnsCorrectValues)
{
    ASSERT_FALSE(settings.isLocked());

    settings.lock();
    ASSERT_TRUE(settings.isLocked());

    settings.unlock();
    ASSERT_FALSE(settings.isLocked());
}

TEST(GlobalSettingsTest, DiscTypeDistributionCantBeEmpty)
{
    auto distribution = settings.getSettings().discTypeDistribution_;
    distribution.clear();

    ASSERT_ANY_THROW(settings.setDiscTypeDistribution(distribution));
}

TEST(GlobalSettingsTest, DiscTypeDistributionPercentagesAddUpTo100)
{
    for (int i = -10; i < 200; i += 10)
    {
        std::map<DiscType, int> distribution{{A, i}};

        if (i == 100)
            ASSERT_NO_THROW(settings.setDiscTypeDistribution(distribution));
        else
            ASSERT_ANY_THROW(settings.setDiscTypeDistribution(distribution));
    }
}

TEST(GlobalSettingsTest, DuplicateNamesInDistributionArentAllowed)
{
    std::map<DiscType, int> distribution{{A, 50}, {ACopy, 50}};

    ASSERT_ANY_THROW(settings.setDiscTypeDistribution(distribution));
}

void insertDefaultReactions(GlobalSettings& settings)
{
    settings.addReaction(decomposition);
    settings.addReaction(combination);
    settings.addReaction(exchange);
}

TEST(GlobalSettingsTest, ClearReactionsClearsReactions)
{
    insertDefaultReactions(settings);

    settings.clearReactions();

    ASSERT_TRUE(settings.getSettings().decompositionReactions_.empty());
    ASSERT_TRUE(settings.getSettings().combinationReactions_.empty());
    ASSERT_TRUE(settings.getSettings().exchangeReactions_.empty());
}

TEST(GlobalSettingsTest, ReactionsEndUpInTheRightPlace)
{
    settings.clearReactions();

    insertDefaultReactions(settings);

    ASSERT_EQ(settings.getSettings().decompositionReactions_.size(), 1);
    ASSERT_EQ(settings.getSettings().decompositionReactions_.begin()->second.front(), decomposition);

    ASSERT_EQ(settings.getSettings().combinationReactions_.size(), 1);
    ASSERT_EQ(settings.getSettings().combinationReactions_.begin()->second.front(), combination);

    ASSERT_EQ(settings.getSettings().exchangeReactions_.size(), 1);
    ASSERT_EQ(settings.getSettings().exchangeReactions_.begin()->second.front(), exchange);
}

TEST(GlobalSettingsTest, DuplicateReactionsArentAllowed)
{
    settings.clearReactions();

    insertDefaultReactions(settings);

    ASSERT_ANY_THROW(settings.addReaction(decomposition));
    ASSERT_ANY_THROW(settings.addReaction(combination));
    ASSERT_ANY_THROW(settings.addReaction(exchange));
}

TEST(GlobalSettingsTest, ReactionsWithIdenticalEductsArentDuplicated)
{
    settings.clearReactions();

    Reaction noDuplicateCombination{A, B, A, std::nullopt, 0.1f};
    Reaction noDuplicateExchange{A, B, A, B, 0.1f};

    // These reactions should be added twice for keys {A, B} and {B, A} for easier lookup
    settings.addReaction(noDuplicateCombination);
    settings.addReaction(noDuplicateExchange);

    ASSERT_EQ(settings.getSettings().combinationReactions_.size(), 2);
    ASSERT_EQ(settings.getSettings().exchangeReactions_.size(), 2);
    settings.clearReactions();

    // Here we have reactions with keys {A, A} -> Shouldn't be duplicated
    insertDefaultReactions(settings);

    ASSERT_EQ(settings.getSettings().combinationReactions_.size(), 1);
    ASSERT_EQ(settings.getSettings().exchangeReactions_.size(), 1);
}

template <typename T> bool isInEductsOrProducts(const T& reactionTable, const DiscType& discType)
{
    for (const auto& [educt, reactions] : reactionTable)
    {
        for (const auto& reaction : reactions)
        {
            if (reaction.getEduct1() == discType || reaction.getProduct1() == discType ||
                (reaction.hasEduct2() && reaction.getEduct2() == discType) ||
                (reaction.hasProduct2() && reaction.getProduct2() == discType))
                return true;
        }
    }

    return false;
}

TEST(GlobalSettingsTest, ReactionWithRemovedDiscTypesAreRemoved)
{
    std::map<DiscType, int> distribution{{A, 100}, {B, 0}, {C, 0}};
    settings.setDiscTypeDistribution(distribution);

    // 1 reaction with A as educt, 1 with A as product, 1 with A nowhere, for each reaction type
    Reaction decompositionAEduct{A, std::nullopt, C, std::nullopt, 0.1f};
    Reaction decompositionAProduct{B, std::nullopt, A, std::nullopt, 0.1f};
    Reaction decompositionANowhere{B, std::nullopt, C, std::nullopt, 0.1f};

    Reaction combinationAEduct{A, B, C, std::nullopt, 0.1f};
    Reaction combinationAProduct{B, C, A, std::nullopt, 0.1f};
    Reaction combinationANowhere{B, C, C, std::nullopt, 0.1f};

    Reaction exchangeAEduct{A, B, C, B, 0.1f};
    Reaction exchangeAProduct{B, C, A, A, 0.1f};
    Reaction exchangeANowhere{B, C, C, C, 0.1f};

    settings.clearReactions();

    std::vector<Reaction> reactions = {decompositionAEduct, decompositionAProduct, decompositionANowhere,
                                       combinationAEduct,   combinationAProduct,   combinationANowhere,
                                       exchangeAEduct,      exchangeAProduct,      exchangeANowhere};

    for (const auto& reaction : reactions)
        settings.addReaction(reaction);

    // Make sure they were all added (twice because educts were not identical for combination/exchange reactions)
    ASSERT_EQ(settings.getSettings().decompositionReactions_.size(), 3);
    ASSERT_EQ(settings.getSettings().combinationReactions_.size(), 6);
    ASSERT_EQ(settings.getSettings().exchangeReactions_.size(), 6);

    // Now remove A from the distribution
    distribution = {{B, 100}, {C, 0}};

    settings.setDiscTypeDistribution(distribution);

    ASSERT_EQ(settings.getSettings().decompositionReactions_.size(), 1);
    ASSERT_FALSE(isInEductsOrProducts(settings.getSettings().decompositionReactions_, A));

    ASSERT_EQ(settings.getSettings().combinationReactions_.size(), 2);
    ASSERT_FALSE(isInEductsOrProducts(settings.getSettings().combinationReactions_, A));

    ASSERT_EQ(settings.getSettings().exchangeReactions_.size(), 2);
    ASSERT_FALSE(isInEductsOrProducts(settings.getSettings().exchangeReactions_, A));
}

TEST(GlobalSettingsTest, DiscTypesInReactionsAreUpdated)
{
}

TEST(GlobalSettingsTest, CantAddReactionsWithDiscTypesThatArentInDistribution)
{
}

TEST(GlobalSettingsTest, KeysAreIdenticalToEducts)
{
}

TEST(GlobalSettingsTest, NoEmptyReactionVectors)
{
}