#include "GlobalSettings.hpp"
#include "TestUtils.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cmath>

using namespace testing;
using namespace cell;

namespace
{

const Reaction decomposition{&Mass10, nullptr, &Mass5, &Mass5, 0.1f};
const Reaction combination{&Mass5, &Mass5, &Mass10, nullptr, 0.1f};
const Reaction exchange{&Mass5, &Mass5, &Mass5, &Mass5, 0.1f};
const Reaction transformation{&Mass5, nullptr, &Mass5Radius10, nullptr, 0.1f};

const DiscType::map<int> DefaultDistribution{{&Mass5, 100},        {&Mass10, 0},       {&Mass15, 0},
                                             {&Mass20, 0},         {&Mass25, 0},       {&Mass5Radius10, 0},
                                             {&Mass10Radius10, 0}, {&Mass10Radius5, 0}};

const Settings& settings = GlobalSettings::getSettings();
const auto& transformationReactions = GlobalSettings::getSettings().reactionTable_.getTransformationReactionLookupMap();
const auto& decompositionReactions = GlobalSettings::getSettings().reactionTable_.getDecompositionReactionLookupMap();
const auto& combinationReactions = GlobalSettings::getSettings().reactionTable_.getCombinationReactionLookupMap();
const auto& exchangeReactions = GlobalSettings::getSettings().reactionTable_.getExchangeReactionLookupMap();

void insertDefaultReactions(GlobalSettings& target)
{
    target.clearReactions();
    target.setDiscTypeDistribution(DefaultDistribution);

    target.addReaction(transformation);
    target.addReaction(decomposition);
    target.addReaction(combination);
    target.addReaction(exchange);
}

template <typename T> bool isInEductsOrProducts(const T& reactionTable, const DiscType* discType)
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

} // namespace

TEST(GlobalSettingsTest, RangeChecksAreCorrect)
{
    GlobalSettings& globalSettings = GlobalSettings::get();
    using namespace SettingsLimits;

    auto minTimeUs = MinSimulationTimeStep.asMicroseconds();
    auto maxTimeUs = MaxSimulationTimeStep.asMicroseconds();
    EXPECT_ANY_THROW(globalSettings.setSimulationTimeStep(sf::microseconds(minTimeUs - 1)));
    EXPECT_ANY_THROW(globalSettings.setSimulationTimeStep(sf::microseconds(maxTimeUs + 1)));
    EXPECT_NO_THROW(globalSettings.setSimulationTimeStep(sf::microseconds(minTimeUs)));
    EXPECT_NO_THROW(globalSettings.setSimulationTimeStep(sf::microseconds(maxTimeUs)));

    EXPECT_ANY_THROW(
        globalSettings.setSimulationTimeScale(std::nextafter(MinSimulationTimeScale, MinSimulationTimeScale - 1)));
    EXPECT_ANY_THROW(
        globalSettings.setSimulationTimeScale(std::nextafter(MaxSimulationTimeScale, MaxSimulationTimeScale + 1)));
    EXPECT_NO_THROW(globalSettings.setSimulationTimeScale(MinSimulationTimeScale));
    EXPECT_NO_THROW(globalSettings.setSimulationTimeScale(MaxSimulationTimeScale));

    EXPECT_ANY_THROW(globalSettings.setNumberOfDiscs(MinNumberOfDiscs - 1));
    EXPECT_ANY_THROW(globalSettings.setNumberOfDiscs(MaxNumberOfDiscs + 1));
    EXPECT_NO_THROW(globalSettings.setNumberOfDiscs(MinNumberOfDiscs));
    EXPECT_NO_THROW(globalSettings.setNumberOfDiscs(MaxNumberOfDiscs));
}

TEST(GlobalSettingsTest, CallbackIsExecuted)
{
    GlobalSettings& globalSettings = GlobalSettings::get();

    SettingID id = SettingID::DiscTypeDistribution;
    const auto& callback = [&](const SettingID& settingID) { id = settingID; };
    globalSettings.setCallback(callback);

    globalSettings.setNumberOfDiscs(50);

    EXPECT_EQ(id, SettingID::NumberOfDiscs);

    globalSettings.setCallback({});
}

TEST(GlobalSettingsTest, LockPreventsChanges)
{
    GlobalSettings& globalSettings = GlobalSettings::get();

    globalSettings.lock();
    globalSettings.unlock();

    // Unlocked: all modification calls succeed.
    EXPECT_NO_THROW(globalSettings.setSimulationTimeStep(SettingsLimits::MinSimulationTimeStep));
    EXPECT_NO_THROW(globalSettings.setSimulationTimeScale(SettingsLimits::MinSimulationTimeScale));
    EXPECT_NO_THROW(globalSettings.setNumberOfDiscs(SettingsLimits::MinNumberOfDiscs));

    EXPECT_NO_THROW(globalSettings.setDiscTypeDistribution(DefaultDistribution));

    EXPECT_NO_THROW(globalSettings.addReaction(decomposition));
    EXPECT_NO_THROW(globalSettings.clearReactions());

    // Lock the globalSettings so that further changes are prohibited.
    globalSettings.lock();

    // Now all modification calls should throw ExceptionWithLocation.
    EXPECT_THROW(globalSettings.setSimulationTimeStep(SettingsLimits::MinSimulationTimeStep), std::runtime_error);
    EXPECT_THROW(globalSettings.setSimulationTimeScale(SettingsLimits::MinSimulationTimeScale), std::runtime_error);
    EXPECT_THROW(globalSettings.setNumberOfDiscs(SettingsLimits::MinNumberOfDiscs), std::runtime_error);
    EXPECT_THROW(globalSettings.setDiscTypeDistribution(DefaultDistribution), std::runtime_error);
    EXPECT_THROW(globalSettings.addReaction(decomposition), std::runtime_error);
    EXPECT_THROW(globalSettings.clearReactions(), std::runtime_error);

    globalSettings.unlock();
}

TEST(GlobalSettingsTest, IsLockedReturnsCorrectValues)
{
    GlobalSettings& globalSettings = GlobalSettings::get();

    EXPECT_FALSE(globalSettings.isLocked());

    globalSettings.lock();
    EXPECT_TRUE(globalSettings.isLocked());

    globalSettings.unlock();
    EXPECT_FALSE(globalSettings.isLocked());
}

TEST(GlobalSettingsTest, DiscTypeDistributionCantBeEmpty)
{
    GlobalSettings& globalSettings = GlobalSettings::get();
    auto distribution = settings.discTypeDistribution_;
    distribution.clear();

    EXPECT_ANY_THROW(globalSettings.setDiscTypeDistribution(distribution));
}

TEST(GlobalSettingsTest, DiscTypeDistributionPercentagesAddUpTo100)
{
    GlobalSettings& globalSettings = GlobalSettings::get();

    for (int i = -10; i < 200; i += 10)
    {
        DiscType::map<int> distribution{{&Mass5, i}};

        if (i == 100)
            EXPECT_NO_THROW(globalSettings.setDiscTypeDistribution(distribution));
        else
            EXPECT_ANY_THROW(globalSettings.setDiscTypeDistribution(distribution));
    }
}

TEST(GlobalSettingsTest, DuplicateNamesInDistributionArentAllowed)
{
    GlobalSettings& globalSettings = GlobalSettings::get();

    DiscType::map<int> distribution{{&Mass5, 50}, {&Mass5, 50}};

    EXPECT_ANY_THROW(globalSettings.setDiscTypeDistribution(distribution));
}

TEST(GlobalSettingsTest, ClearReactionsClearsReactions)
{
    GlobalSettings& globalSettings = GlobalSettings::get();

    insertDefaultReactions(globalSettings);

    globalSettings.clearReactions();

    EXPECT_TRUE(transformationReactions.empty());
    EXPECT_TRUE(decompositionReactions.empty());
    EXPECT_TRUE(combinationReactions.empty());
    EXPECT_TRUE(exchangeReactions.empty());
}

TEST(GlobalSettingsTest, ReactionsEndUpInTheRightPlace)
{
    GlobalSettings& globalSettings = GlobalSettings::get();

    globalSettings.clearReactions();

    insertDefaultReactions(globalSettings);

    EXPECT_EQ(transformationReactions.size(), 1);
    EXPECT_EQ(transformationReactions.begin()->second.front(), transformation);

    EXPECT_EQ(decompositionReactions.size(), 1);
    EXPECT_EQ(decompositionReactions.begin()->second.front(), decomposition);

    EXPECT_EQ(combinationReactions.size(), 1);
    EXPECT_EQ(combinationReactions.begin()->second.front(), combination);

    EXPECT_EQ(exchangeReactions.size(), 1);
    EXPECT_EQ(exchangeReactions.begin()->second.front(), exchange);
}

TEST(GlobalSettingsTest, DuplicateReactionsArentAllowed)
{
    GlobalSettings& globalSettings = GlobalSettings::get();

    globalSettings.clearReactions();

    insertDefaultReactions(globalSettings);

    EXPECT_ANY_THROW(globalSettings.addReaction(transformation));
    EXPECT_ANY_THROW(globalSettings.addReaction(decomposition));
    EXPECT_ANY_THROW(globalSettings.addReaction(combination));
    EXPECT_ANY_THROW(globalSettings.addReaction(exchange));
}

TEST(GlobalSettingsTest, ReactionsWithIdenticalEductsArentDuplicated)
{
    GlobalSettings& globalSettings = GlobalSettings::get();

    globalSettings.clearReactions();
    globalSettings.setDiscTypeDistribution({{&Mass5, 50}, {&Mass10, 50}, {&Mass15, 0}});

    Reaction noDuplicateCombination{&Mass5, &Mass10, &Mass15, nullptr, 0.1f};
    Reaction noDuplicateExchange{&Mass5, &Mass10, &Mass5, &Mass10, 0.1f};

    // These reactions should be added twice for keys {A, B} and {B, A} for easier lookup
    globalSettings.addReaction(noDuplicateCombination);
    globalSettings.addReaction(noDuplicateExchange);

    EXPECT_EQ(combinationReactions.size(), 2);
    EXPECT_EQ(exchangeReactions.size(), 2);
    globalSettings.clearReactions();

    // Here we have reactions with keys {A, A} -> Shouldn't be duplicated
    insertDefaultReactions(globalSettings);

    EXPECT_EQ(combinationReactions.size(), 1);
    EXPECT_EQ(combinationReactions.begin()->second.size(), 1);
    EXPECT_EQ(exchangeReactions.size(), 1);
    EXPECT_EQ(exchangeReactions.begin()->second.size(), 1);
}

TEST(GlobalSettingsTest, ReactionsWithRemovedDiscTypesAreRemoved)
{
    GlobalSettings& globalSettings = GlobalSettings::get();

    globalSettings.setDiscTypeDistribution(DefaultDistribution);

    // 1 reaction with &Mass10 as educt, 1 with &Mass10 as product, 1 with &Mass10 nowhere, for each reaction type
    // A = &Mass10 for notation

    Reaction transformationAEduct{&Mass10, nullptr, &Mass10Radius10, nullptr, 0.1f};
    Reaction transformationAProduct{&Mass10Radius10, nullptr, &Mass10, nullptr, 0.1f};
    Reaction transformationANowhere{&Mass10Radius10, nullptr, &Mass10Radius5, nullptr, 0.1f};

    Reaction decompositionAEduct{&Mass10, nullptr, &Mass5, &Mass5, 0.1f};
    Reaction decompositionAProduct{&Mass15, nullptr, &Mass5, &Mass10, 0.1f};
    Reaction decompositionANowhere{&Mass20, nullptr, &Mass15, &Mass5, 0.1f};

    Reaction combinationAEduct{&Mass5, &Mass10, &Mass15, nullptr, 0.1f};
    Reaction combinationAProduct{&Mass5, &Mass5, &Mass10, nullptr, 0.1f};
    Reaction combinationANowhere{&Mass5, &Mass15, &Mass20, nullptr, 0.1f};

    Reaction exchangeAEduct{&Mass10, &Mass15, &Mass5, &Mass20, 0.1f};
    Reaction exchangeAProduct{&Mass5, &Mass20, &Mass10, &Mass15, 0.1f};
    Reaction exchangeANowhere{&Mass15, &Mass15, &Mass25, &Mass5, 0.1f};

    globalSettings.clearReactions();

    // Add all these reactions
    std::vector<Reaction> reactions = {transformationAEduct, transformationAProduct, transformationANowhere,
                                       decompositionAEduct,  decompositionAProduct,  decompositionANowhere,
                                       combinationAEduct,    combinationAProduct,    combinationANowhere,
                                       exchangeAEduct,       exchangeAProduct,       exchangeANowhere};

    for (const auto& reaction : reactions)
        globalSettings.addReaction(reaction);

    // Make sure they were all added (some twice because educts were not identical for combination/exchange reactions)

    EXPECT_EQ(transformationReactions.size(), 2);
    EXPECT_EQ(decompositionReactions.size(), 3);
    EXPECT_EQ(combinationReactions.size(), 5);
    EXPECT_EQ(exchangeReactions.size(), 5);

    // Now remove &Mass10 from the distribution
    DiscType::map<int> distribution = DefaultDistribution;
    distribution.erase(&Mass10);

    globalSettings.setDiscTypeDistribution(distribution);

    EXPECT_EQ(transformationReactions.size(), 1);
    EXPECT_FALSE(isInEductsOrProducts(transformationReactions, &Mass10));

    EXPECT_EQ(decompositionReactions.size(), 1);
    EXPECT_FALSE(isInEductsOrProducts(decompositionReactions, &Mass10));

    EXPECT_EQ(combinationReactions.size(), 2);
    EXPECT_FALSE(isInEductsOrProducts(combinationReactions, &Mass10));

    EXPECT_EQ(exchangeReactions.size(), 1);
    EXPECT_FALSE(isInEductsOrProducts(exchangeReactions, &Mass10));

    // Remove all disc types -> All reactions should be cleared
    globalSettings.setDiscTypeDistribution({{&Reserved, 100}});

    EXPECT_TRUE(transformationReactions.empty());
    EXPECT_TRUE(decompositionReactions.empty());
    EXPECT_TRUE(combinationReactions.empty());
    EXPECT_TRUE(exchangeReactions.empty());
}

TEST(GlobalSettingsTest, CantAddReactionsWithDiscTypesThatArentInDistribution)
{
    GlobalSettings& globalSettings = GlobalSettings::get();

    DiscType Mass50{"a", sf::Color::Cyan, 2.1f, 50};
    DiscType Mass50Radius100{"b", sf::Color::Cyan, 100, 50};
    DiscType Mass100{"LOOL", sf::Color::Cyan, 2.1f, 100};

    EXPECT_ANY_THROW(globalSettings.addReaction(Reaction{&Mass50, nullptr, &Mass50Radius100, nullptr, 0.1f}));
    EXPECT_ANY_THROW(globalSettings.addReaction(Reaction{&Mass100, nullptr, &Mass50, &Mass50, 0.1f}));
    EXPECT_ANY_THROW(globalSettings.addReaction(Reaction{&Mass50, &Mass50, &Mass100, nullptr, 0.1f}));
    EXPECT_ANY_THROW(globalSettings.addReaction(Reaction{&Mass50, &Mass50, &Mass50, &Mass50, 0.1f}));
}

TEST(GlobalSettings, HaveNoDiscTypesIfNoneWereAdded)
{
    ASSERT_THAT(GlobalSettings::getSettings().discTypes_.empty(), Eq(true));
}