#include <gtest/gtest.h>

#include "GlobalSettings.hpp"

#include <cmath>

namespace
{

const DiscType Mass5{"A", sf::Color::Green, DiscTypeLimits::MinRadius, 5};
const DiscType Mass10{"B", sf::Color::Green, DiscTypeLimits::MinRadius, 10};
const DiscType Mass15{"C", sf::Color::Green, DiscTypeLimits::MinRadius, 15};
const DiscType Mass20{"D", sf::Color::Green, DiscTypeLimits::MinRadius, 20};
const DiscType Mass25{"E", sf::Color::Green, DiscTypeLimits::MinRadius, 25};
const DiscType Unused{"F", sf::Color::Green, DiscTypeLimits::MinRadius, 30};

const Reaction decomposition{Mass10, std::nullopt, Mass5, Mass5, 0.1f};
const Reaction combination{Mass5, Mass5, Mass10, std::nullopt, 0.1f};
const Reaction exchange{Mass5, Mass5, Mass5, Mass5, 0.1f};

const DiscType::map<int> DefaultDistribution{{Mass5, 100}, {Mass10, 0}, {Mass15, 0}, {Mass20, 0}, {Mass25, 0}};

GlobalSettings& settings = GlobalSettings::get();

void insertDefaultReactions(GlobalSettings& globalSettings)
{
    globalSettings.clearReactions();
    globalSettings.setDiscTypeDistribution(DefaultDistribution);
    globalSettings.addReaction(decomposition);
    globalSettings.addReaction(combination);
    globalSettings.addReaction(exchange);
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

template <typename T> int countDiscTypeInReactionTable(const T& reactionTable, const DiscType& discType)
{
    using KeyType = T::key_type;

    int count = 0;
    for (const auto& [educts, reactions] : reactionTable)
    {
        if constexpr (std::is_same_v<KeyType, std::pair<DiscType, DiscType>>)
        {
            if (educts.first == discType)
                ++count;
            if (educts.second == discType)
                ++count;
        }
        else
        {
            if (educts == discType)
                ++count;
        }

        for (const auto& reaction : reactions)
        {
            if (reaction.getEduct1() == discType)
                ++count;
            if (reaction.hasEduct2() && reaction.getEduct2() == discType)
                ++count;
            if (reaction.getProduct1() == discType)
                ++count;
            if (reaction.hasProduct2() && reaction.getProduct2() == discType)
                ++count;
        }
    }

    return count;
}

int countDiscTypeInReactions(const GlobalSettings& globalSettings, const DiscType& discType)
{
    int count = 0;
    count += countDiscTypeInReactionTable(globalSettings.getSettings().combinationReactions_, discType);
    count += countDiscTypeInReactionTable(globalSettings.getSettings().decompositionReactions_, discType);
    count += countDiscTypeInReactionTable(globalSettings.getSettings().exchangeReactions_, discType);

    return count;
}

} // namespace

TEST(GlobalSettingsTest, RangeChecksAreCorrect)
{
    using namespace SettingsLimits;

    EXPECT_ANY_THROW(
        settings.setFrictionCoefficient(std::nextafterf(MinFrictionCoefficient, MinFrictionCoefficient - 1)));
    EXPECT_ANY_THROW(
        settings.setFrictionCoefficient(std::nextafterf(MaxFrictionCoefficient, MaxFrictionCoefficient + 1)));
    EXPECT_NO_THROW(settings.setFrictionCoefficient(MinFrictionCoefficient));
    EXPECT_NO_THROW(settings.setFrictionCoefficient(MaxFrictionCoefficient));

    int minTimeUs = MinSimulationTimeStep.asMicroseconds();
    int maxTimeUs = MaxSimulationTimeStep.asMicroseconds();
    EXPECT_ANY_THROW(settings.setSimulationTimeStep(sf::microseconds(minTimeUs - 1)));
    EXPECT_ANY_THROW(settings.setSimulationTimeStep(sf::microseconds(maxTimeUs + 1)));
    EXPECT_NO_THROW(settings.setSimulationTimeStep(sf::microseconds(minTimeUs)));
    EXPECT_NO_THROW(settings.setSimulationTimeStep(sf::microseconds(maxTimeUs)));

    EXPECT_ANY_THROW(
        settings.setSimulationTimeScale(std::nextafterf(MinSimulationTimeScale, MinSimulationTimeScale - 1)));
    EXPECT_ANY_THROW(
        settings.setSimulationTimeScale(std::nextafterf(MaxSimulationTimeScale, MaxSimulationTimeScale + 1)));
    EXPECT_NO_THROW(settings.setSimulationTimeScale(MinSimulationTimeScale));
    EXPECT_NO_THROW(settings.setSimulationTimeScale(MaxSimulationTimeScale));

    EXPECT_ANY_THROW(settings.setNumberOfDiscs(MinNumberOfDiscs - 1));
    EXPECT_ANY_THROW(settings.setNumberOfDiscs(MaxNumberOfDiscs + 1));
    EXPECT_NO_THROW(settings.setNumberOfDiscs(MinNumberOfDiscs));
    EXPECT_NO_THROW(settings.setNumberOfDiscs(MaxNumberOfDiscs));
}

TEST(GlobalSettingsTest, CallbackIsExecuted)
{
    SettingID id = SettingID::DiscTypeDistribution;
    const auto& callback = [&](const SettingID& settingID) { id = settingID; };
    settings.setCallback(callback);

    settings.setFrictionCoefficient(SettingsLimits::MinFrictionCoefficient);

    EXPECT_EQ(id, SettingID::FrictionCoefficient);

    settings.setCallback({});
}

TEST(GlobalSettingsTest, LockPreventsChanges)
{
    settings.lock();
    settings.unlock();

    // Unlocked: all modification calls succeed.
    EXPECT_NO_THROW(settings.setSimulationTimeStep(SettingsLimits::MinSimulationTimeStep));
    EXPECT_NO_THROW(settings.setSimulationTimeScale(SettingsLimits::MinSimulationTimeScale));
    EXPECT_NO_THROW(settings.setNumberOfDiscs(SettingsLimits::MinNumberOfDiscs));
    EXPECT_NO_THROW(settings.setFrictionCoefficient(SettingsLimits::MinFrictionCoefficient));

    EXPECT_NO_THROW(settings.setDiscTypeDistribution(DefaultDistribution));

    EXPECT_NO_THROW(settings.addReaction(decomposition));
    EXPECT_NO_THROW(settings.clearReactions());

    // Lock the settings so that further changes are prohibited.
    settings.lock();

    // Now all modification calls should throw ExceptionWithLocation.
    EXPECT_THROW(settings.setSimulationTimeStep(SettingsLimits::MinSimulationTimeStep), std::runtime_error);
    EXPECT_THROW(settings.setSimulationTimeScale(SettingsLimits::MinSimulationTimeScale), std::runtime_error);
    EXPECT_THROW(settings.setNumberOfDiscs(SettingsLimits::MinNumberOfDiscs), std::runtime_error);
    EXPECT_THROW(settings.setFrictionCoefficient(SettingsLimits::MinFrictionCoefficient), std::runtime_error);
    EXPECT_THROW(settings.setDiscTypeDistribution(DefaultDistribution), std::runtime_error);
    EXPECT_THROW(settings.addReaction(decomposition), std::runtime_error);
    EXPECT_THROW(settings.clearReactions(), std::runtime_error);

    settings.unlock();
}

TEST(GlobalSettingsTest, IsLockedReturnsCorrectValues)
{
    EXPECT_FALSE(settings.isLocked());

    settings.lock();
    EXPECT_TRUE(settings.isLocked());

    settings.unlock();
    EXPECT_FALSE(settings.isLocked());
}

TEST(GlobalSettingsTest, DiscTypeDistributionCantBeEmpty)
{
    auto distribution = settings.getSettings().discTypeDistribution_;
    distribution.clear();

    EXPECT_ANY_THROW(settings.setDiscTypeDistribution(distribution));
}

TEST(GlobalSettingsTest, DiscTypeDistributionPercentagesAddUpTo100)
{
    for (int i = -10; i < 200; i += 10)
    {
        DiscType::map<int> distribution{{Mass5, i}};

        if (i == 100)
            EXPECT_NO_THROW(settings.setDiscTypeDistribution(distribution));
        else
            EXPECT_ANY_THROW(settings.setDiscTypeDistribution(distribution));
    }
}

TEST(GlobalSettingsTest, DuplicateNamesInDistributionArentAllowed)
{
    DiscType::map<int> distribution{{Mass5, 50}, {Mass5, 50}};

    EXPECT_ANY_THROW(settings.setDiscTypeDistribution(distribution));
}

TEST(GlobalSettingsTest, ClearReactionsClearsReactions)
{
    insertDefaultReactions(settings);

    settings.clearReactions();

    EXPECT_TRUE(settings.getSettings().decompositionReactions_.empty());
    EXPECT_TRUE(settings.getSettings().combinationReactions_.empty());
    EXPECT_TRUE(settings.getSettings().exchangeReactions_.empty());
}

TEST(GlobalSettingsTest, ReactionsEndUpInTheRightPlace)
{
    settings.clearReactions();

    insertDefaultReactions(settings);

    EXPECT_EQ(settings.getSettings().decompositionReactions_.size(), 1);
    EXPECT_EQ(settings.getSettings().decompositionReactions_.begin()->second.front(), decomposition);

    EXPECT_EQ(settings.getSettings().combinationReactions_.size(), 1);
    EXPECT_EQ(settings.getSettings().combinationReactions_.begin()->second.front(), combination);

    EXPECT_EQ(settings.getSettings().exchangeReactions_.size(), 1);
    EXPECT_EQ(settings.getSettings().exchangeReactions_.begin()->second.front(), exchange);
}

TEST(GlobalSettingsTest, DuplicateReactionsArentAllowed)
{
    settings.clearReactions();

    insertDefaultReactions(settings);

    EXPECT_ANY_THROW(settings.addReaction(decomposition));
    EXPECT_ANY_THROW(settings.addReaction(combination));
    EXPECT_ANY_THROW(settings.addReaction(exchange));
}

TEST(GlobalSettingsTest, ReactionsWithIdenticalEductsArentDuplicated)
{
    settings.clearReactions();
    settings.setDiscTypeDistribution({{Mass5, 50}, {Mass10, 50}, {Mass15, 0}});

    Reaction noDuplicateCombination{Mass5, Mass10, Mass15, std::nullopt, 0.1f};
    Reaction noDuplicateExchange{Mass5, Mass10, Mass5, Mass10, 0.1f};

    // These reactions should be added twice for keys {A, B} and {B, A} for easier lookup
    settings.addReaction(noDuplicateCombination);
    settings.addReaction(noDuplicateExchange);

    EXPECT_EQ(settings.getSettings().combinationReactions_.size(), 2);
    EXPECT_EQ(settings.getSettings().exchangeReactions_.size(), 2);
    settings.clearReactions();

    // Here we have reactions with keys {A, A} -> Shouldn't be duplicated
    insertDefaultReactions(settings);

    EXPECT_EQ(settings.getSettings().combinationReactions_.size(), 1);
    EXPECT_EQ(settings.getSettings().combinationReactions_.begin()->second.size(), 1);
    EXPECT_EQ(settings.getSettings().exchangeReactions_.size(), 1);
    EXPECT_EQ(settings.getSettings().exchangeReactions_.begin()->second.size(), 1);
}

TEST(GlobalSettingsTest, ReactionWithRemovedDiscTypesAreRemoved)
{
    settings.setDiscTypeDistribution(DefaultDistribution);

    // 1 reaction with Mass10 as educt, 1 with Mass10 as product, 1 with Mass10 nowhere, for each reaction type
    // A = Mass10 for notation
    Reaction decompositionAEduct{Mass10, std::nullopt, Mass5, Mass5, 0.1f};
    Reaction decompositionAProduct{Mass15, std::nullopt, Mass5, Mass10, 0.1f};
    Reaction decompositionANowhere{Mass20, std::nullopt, Mass15, Mass5, 0.1f};

    Reaction combinationAEduct{Mass5, Mass10, Mass15, std::nullopt, 0.1f};
    Reaction combinationAProduct{Mass5, Mass5, Mass10, std::nullopt, 0.1f};
    Reaction combinationANowhere{Mass5, Mass15, Mass20, std::nullopt, 0.1f};

    Reaction exchangeAEduct{Mass10, Mass15, Mass5, Mass20, 0.1f};
    Reaction exchangeAProduct{Mass5, Mass20, Mass10, Mass15, 0.1f};
    Reaction exchangeANowhere{Mass15, Mass15, Mass25, Mass5, 0.1f};

    settings.clearReactions();

    // Add all these reactions
    std::vector<Reaction> reactions = {decompositionAEduct, decompositionAProduct, decompositionANowhere,
                                       combinationAEduct,   combinationAProduct,   combinationANowhere,
                                       exchangeAEduct,      exchangeAProduct,      exchangeANowhere};

    for (const auto& reaction : reactions)
        settings.addReaction(reaction);

    // Make sure they were all added (some twice because educts were not identical for combination/exchange reactions)

    EXPECT_EQ(settings.getSettings().decompositionReactions_.size(), 3);
    EXPECT_EQ(settings.getSettings().combinationReactions_.size(), 5);
    EXPECT_EQ(settings.getSettings().exchangeReactions_.size(), 5);

    // Now remove Mass10 from the distribution
    DiscType::map<int> distribution = DefaultDistribution;
    distribution.erase(Mass10);

    settings.setDiscTypeDistribution(distribution);

    EXPECT_EQ(settings.getSettings().decompositionReactions_.size(), 1);
    EXPECT_FALSE(isInEductsOrProducts(settings.getSettings().decompositionReactions_, Mass10));

    EXPECT_EQ(settings.getSettings().combinationReactions_.size(), 2);
    EXPECT_FALSE(isInEductsOrProducts(settings.getSettings().combinationReactions_, Mass10));

    EXPECT_EQ(settings.getSettings().exchangeReactions_.size(), 1);
    EXPECT_FALSE(isInEductsOrProducts(settings.getSettings().exchangeReactions_, Mass10));

    // Remove all disc types -> All reactions should be cleared
    settings.setDiscTypeDistribution({{Unused, 100}});

    EXPECT_TRUE(settings.getSettings().decompositionReactions_.empty());
    EXPECT_TRUE(settings.getSettings().combinationReactions_.empty());
    EXPECT_TRUE(settings.getSettings().exchangeReactions_.empty());
}

TEST(GlobalSettingsTest, DiscTypesInReactionsAreUpdated)
{
    settings.setDiscTypeDistribution(DefaultDistribution);
    settings.clearReactions();
    insertDefaultReactions(settings);

    int count = countDiscTypeInReactions(settings, Mass5);

    DiscType Mass5Modified = Mass5;
    Mass5Modified.setName("Modified");
    DiscType::map<int> distribution = DefaultDistribution;
    distribution.erase(Mass5);
    distribution[Mass5Modified] = 100;
    settings.setDiscTypeDistribution(distribution);

    EXPECT_EQ(count, countDiscTypeInReactions(settings, Mass5Modified));
}

TEST(GlobalSettingsTest, CantAddReactionsWithDiscTypesThatArentInDistribution)
{
    DiscType Mass50{"LOL", sf::Color::Cyan, 2.1f, 50};
    DiscType Mass100{"LOOL", sf::Color::Cyan, 2.1f, 100};

    EXPECT_ANY_THROW(settings.addReaction(Reaction{Mass100, std::nullopt, Mass50, Mass50, 0.1f}));
    EXPECT_ANY_THROW(settings.addReaction(Reaction{Mass50, Mass50, Mass100, std::nullopt, 0.1f}));
    EXPECT_ANY_THROW(settings.addReaction(Reaction{Mass50, Mass50, Mass50, Mass50, 0.1f}));
}
