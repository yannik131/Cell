#include "GlobalSettings.hpp"
#include "TestUtils.hpp"

#include <gtest/gtest.h>

#include <cmath>

namespace
{

const Reaction decomposition{Mass10, std::nullopt, Mass5, Mass5, 0.1f};
const Reaction combination{Mass5, Mass5, Mass10, std::nullopt, 0.1f};
const Reaction exchange{Mass5, Mass5, Mass5, Mass5, 0.1f};
const Reaction transformation{Mass5, std::nullopt, Mass5Radius10, std::nullopt, 0.1f};

const DiscType::map<int> DefaultDistribution{{Mass5, 100}, {Mass10, 0},        {Mass15, 0},         {Mass20, 0},
                                             {Mass25, 0},  {Mass5Radius10, 0}, {Mass10Radius10, 0}, {Mass10Radius5, 0}};

GlobalSettings& globalSettings = GlobalSettings::get();
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

int countDiscTypeInReactions(const DiscType& discType)
{
    int count = 0;
    count += countDiscTypeInReactionTable(transformationReactions, discType);
    count += countDiscTypeInReactionTable(decompositionReactions, discType);
    count += countDiscTypeInReactionTable(combinationReactions, discType);
    count += countDiscTypeInReactionTable(exchangeReactions, discType);

    return count;
}

} // namespace

TEST(GlobalSettingsTest, RangeChecksAreCorrect)
{
    using namespace SettingsLimits;

    EXPECT_ANY_THROW(
        globalSettings.setFrictionCoefficient(std::nextafterf(MinFrictionCoefficient, MinFrictionCoefficient - 1)));
    EXPECT_ANY_THROW(
        globalSettings.setFrictionCoefficient(std::nextafterf(MaxFrictionCoefficient, MaxFrictionCoefficient + 1)));
    EXPECT_NO_THROW(globalSettings.setFrictionCoefficient(MinFrictionCoefficient));
    EXPECT_NO_THROW(globalSettings.setFrictionCoefficient(MaxFrictionCoefficient));

    int minTimeUs = MinSimulationTimeStep.asMicroseconds();
    int maxTimeUs = MaxSimulationTimeStep.asMicroseconds();
    EXPECT_ANY_THROW(globalSettings.setSimulationTimeStep(sf::microseconds(minTimeUs - 1)));
    EXPECT_ANY_THROW(globalSettings.setSimulationTimeStep(sf::microseconds(maxTimeUs + 1)));
    EXPECT_NO_THROW(globalSettings.setSimulationTimeStep(sf::microseconds(minTimeUs)));
    EXPECT_NO_THROW(globalSettings.setSimulationTimeStep(sf::microseconds(maxTimeUs)));

    EXPECT_ANY_THROW(
        globalSettings.setSimulationTimeScale(std::nextafterf(MinSimulationTimeScale, MinSimulationTimeScale - 1)));
    EXPECT_ANY_THROW(
        globalSettings.setSimulationTimeScale(std::nextafterf(MaxSimulationTimeScale, MaxSimulationTimeScale + 1)));
    EXPECT_NO_THROW(globalSettings.setSimulationTimeScale(MinSimulationTimeScale));
    EXPECT_NO_THROW(globalSettings.setSimulationTimeScale(MaxSimulationTimeScale));

    EXPECT_ANY_THROW(globalSettings.setNumberOfDiscs(MinNumberOfDiscs - 1));
    EXPECT_ANY_THROW(globalSettings.setNumberOfDiscs(MaxNumberOfDiscs + 1));
    EXPECT_NO_THROW(globalSettings.setNumberOfDiscs(MinNumberOfDiscs));
    EXPECT_NO_THROW(globalSettings.setNumberOfDiscs(MaxNumberOfDiscs));
}

TEST(GlobalSettingsTest, CallbackIsExecuted)
{
    SettingID id = SettingID::DiscTypeDistribution;
    const auto& callback = [&](const SettingID& settingID) { id = settingID; };
    globalSettings.setCallback(callback);

    globalSettings.setFrictionCoefficient(SettingsLimits::MinFrictionCoefficient);

    EXPECT_EQ(id, SettingID::FrictionCoefficient);

    globalSettings.setCallback({});
}

TEST(GlobalSettingsTest, LockPreventsChanges)
{
    globalSettings.lock();
    globalSettings.unlock();

    // Unlocked: all modification calls succeed.
    EXPECT_NO_THROW(globalSettings.setSimulationTimeStep(SettingsLimits::MinSimulationTimeStep));
    EXPECT_NO_THROW(globalSettings.setSimulationTimeScale(SettingsLimits::MinSimulationTimeScale));
    EXPECT_NO_THROW(globalSettings.setNumberOfDiscs(SettingsLimits::MinNumberOfDiscs));
    EXPECT_NO_THROW(globalSettings.setFrictionCoefficient(SettingsLimits::MinFrictionCoefficient));

    EXPECT_NO_THROW(globalSettings.setDiscTypeDistribution(DefaultDistribution));

    EXPECT_NO_THROW(globalSettings.addReaction(decomposition));
    EXPECT_NO_THROW(globalSettings.clearReactions());

    // Lock the globalSettings so that further changes are prohibited.
    globalSettings.lock();

    // Now all modification calls should throw ExceptionWithLocation.
    EXPECT_THROW(globalSettings.setSimulationTimeStep(SettingsLimits::MinSimulationTimeStep), std::runtime_error);
    EXPECT_THROW(globalSettings.setSimulationTimeScale(SettingsLimits::MinSimulationTimeScale), std::runtime_error);
    EXPECT_THROW(globalSettings.setNumberOfDiscs(SettingsLimits::MinNumberOfDiscs), std::runtime_error);
    EXPECT_THROW(globalSettings.setFrictionCoefficient(SettingsLimits::MinFrictionCoefficient), std::runtime_error);
    EXPECT_THROW(globalSettings.setDiscTypeDistribution(DefaultDistribution), std::runtime_error);
    EXPECT_THROW(globalSettings.addReaction(decomposition), std::runtime_error);
    EXPECT_THROW(globalSettings.clearReactions(), std::runtime_error);

    globalSettings.unlock();
}

TEST(GlobalSettingsTest, IsLockedReturnsCorrectValues)
{
    EXPECT_FALSE(globalSettings.isLocked());

    globalSettings.lock();
    EXPECT_TRUE(globalSettings.isLocked());

    globalSettings.unlock();
    EXPECT_FALSE(globalSettings.isLocked());
}

TEST(GlobalSettingsTest, DiscTypeDistributionCantBeEmpty)
{
    auto distribution = settings.discTypeDistribution_;
    distribution.clear();

    EXPECT_ANY_THROW(globalSettings.setDiscTypeDistribution(distribution));
}

TEST(GlobalSettingsTest, DiscTypeDistributionPercentagesAddUpTo100)
{
    for (int i = -10; i < 200; i += 10)
    {
        DiscType::map<int> distribution{{Mass5, i}};

        if (i == 100)
            EXPECT_NO_THROW(globalSettings.setDiscTypeDistribution(distribution));
        else
            EXPECT_ANY_THROW(globalSettings.setDiscTypeDistribution(distribution));
    }
}

TEST(GlobalSettingsTest, DuplicateNamesInDistributionArentAllowed)
{
    DiscType::map<int> distribution{{Mass5, 50}, {Mass5, 50}};

    EXPECT_ANY_THROW(globalSettings.setDiscTypeDistribution(distribution));
}

TEST(GlobalSettingsTest, ClearReactionsClearsReactions)
{
    insertDefaultReactions(globalSettings);

    globalSettings.clearReactions();

    EXPECT_TRUE(transformationReactions.empty());
    EXPECT_TRUE(decompositionReactions.empty());
    EXPECT_TRUE(combinationReactions.empty());
    EXPECT_TRUE(exchangeReactions.empty());
}

TEST(GlobalSettingsTest, ReactionsEndUpInTheRightPlace)
{
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
    globalSettings.clearReactions();

    insertDefaultReactions(globalSettings);

    EXPECT_ANY_THROW(globalSettings.addReaction(transformation));
    EXPECT_ANY_THROW(globalSettings.addReaction(decomposition));
    EXPECT_ANY_THROW(globalSettings.addReaction(combination));
    EXPECT_ANY_THROW(globalSettings.addReaction(exchange));
}

TEST(GlobalSettingsTest, ReactionsWithIdenticalEductsArentDuplicated)
{
    globalSettings.clearReactions();
    globalSettings.setDiscTypeDistribution({{Mass5, 50}, {Mass10, 50}, {Mass15, 0}});

    Reaction noDuplicateCombination{Mass5, Mass10, Mass15, std::nullopt, 0.1f};
    Reaction noDuplicateExchange{Mass5, Mass10, Mass5, Mass10, 0.1f};

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

TEST(GlobalSettingsTest, ReactionWithRemovedDiscTypesAreRemoved)
{
    globalSettings.setDiscTypeDistribution(DefaultDistribution);

    // 1 reaction with Mass10 as educt, 1 with Mass10 as product, 1 with Mass10 nowhere, for each reaction type
    // A = Mass10 for notation

    Reaction transformationAEduct{Mass10, std::nullopt, Mass10Radius10, std::nullopt, 0.1f};
    Reaction transformationAProduct{Mass10Radius10, std::nullopt, Mass10, std::nullopt, 0.1f};
    Reaction transformationANowhere{Mass10Radius10, std::nullopt, Mass10Radius5, std::nullopt, 0.1f};

    Reaction decompositionAEduct{Mass10, std::nullopt, Mass5, Mass5, 0.1f};
    Reaction decompositionAProduct{Mass15, std::nullopt, Mass5, Mass10, 0.1f};
    Reaction decompositionANowhere{Mass20, std::nullopt, Mass15, Mass5, 0.1f};

    Reaction combinationAEduct{Mass5, Mass10, Mass15, std::nullopt, 0.1f};
    Reaction combinationAProduct{Mass5, Mass5, Mass10, std::nullopt, 0.1f};
    Reaction combinationANowhere{Mass5, Mass15, Mass20, std::nullopt, 0.1f};

    Reaction exchangeAEduct{Mass10, Mass15, Mass5, Mass20, 0.1f};
    Reaction exchangeAProduct{Mass5, Mass20, Mass10, Mass15, 0.1f};
    Reaction exchangeANowhere{Mass15, Mass15, Mass25, Mass5, 0.1f};

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

    // Now remove Mass10 from the distribution
    DiscType::map<int> distribution = DefaultDistribution;
    distribution.erase(Mass10);

    globalSettings.setDiscTypeDistribution(distribution);

    EXPECT_EQ(transformationReactions.size(), 1);
    EXPECT_FALSE(isInEductsOrProducts(transformationReactions, Mass10));

    EXPECT_EQ(decompositionReactions.size(), 1);
    EXPECT_FALSE(isInEductsOrProducts(decompositionReactions, Mass10));

    EXPECT_EQ(combinationReactions.size(), 2);
    EXPECT_FALSE(isInEductsOrProducts(combinationReactions, Mass10));

    EXPECT_EQ(exchangeReactions.size(), 1);
    EXPECT_FALSE(isInEductsOrProducts(exchangeReactions, Mass10));

    // Remove all disc types -> All reactions should be cleared
    globalSettings.setDiscTypeDistribution({{Unused, 100}});

    EXPECT_TRUE(transformationReactions.empty());
    EXPECT_TRUE(decompositionReactions.empty());
    EXPECT_TRUE(combinationReactions.empty());
    EXPECT_TRUE(exchangeReactions.empty());
}

TEST(GlobalSettingsTest, DiscTypesInReactionsAreUpdated)
{
    globalSettings.setDiscTypeDistribution(DefaultDistribution);
    globalSettings.clearReactions();
    insertDefaultReactions(globalSettings);

    int count = countDiscTypeInReactions(Mass5);

    DiscType Mass5Modified = Mass5;
    Mass5Modified.setName("Modified");
    DiscType::map<int> distribution = DefaultDistribution;
    distribution.erase(Mass5);
    distribution[Mass5Modified] = 100;
    globalSettings.setDiscTypeDistribution(distribution);

    EXPECT_EQ(count, countDiscTypeInReactions(Mass5Modified));
}

TEST(GlobalSettingsTest, CantAddReactionsWithDiscTypesThatArentInDistribution)
{
    DiscType Mass50{"a", sf::Color::Cyan, 2.1f, 50};
    DiscType Mass50Radius100{"b", sf::Color::Cyan, 100, 50};
    DiscType Mass100{"LOOL", sf::Color::Cyan, 2.1f, 100};

    EXPECT_ANY_THROW(globalSettings.addReaction(Reaction{Mass50, std::nullopt, Mass50Radius100, std::nullopt, 0.1f}));
    EXPECT_ANY_THROW(globalSettings.addReaction(Reaction{Mass100, std::nullopt, Mass50, Mass50, 0.1f}));
    EXPECT_ANY_THROW(globalSettings.addReaction(Reaction{Mass50, Mass50, Mass100, std::nullopt, 0.1f}));
    EXPECT_ANY_THROW(globalSettings.addReaction(Reaction{Mass50, Mass50, Mass50, Mass50, 0.1f}));
}
