#include <gtest/gtest.h>

#include "GlobalSettings.hpp"

#include <cmath>

namespace
{

const DiscType A{"A", sf::Color::Green, DiscTypeLimits::MinRadius, DiscTypeLimits::MinMass};
const DiscType ACopy{"A", sf::Color::Green, DiscTypeLimits::MinRadius,
                     DiscTypeLimits::MinMass}; // Same attributes, different ID
const DiscType B{"B", sf::Color::Green, DiscTypeLimits::MinRadius, DiscTypeLimits::MinMass};
const DiscType C{"C", sf::Color::Green, DiscTypeLimits::MinRadius, DiscTypeLimits::MinMass};

const Reaction decomposition{A, std::nullopt, A, A, 0.1f};
const Reaction combination{A, A, A, std::nullopt, 0.1f};
const Reaction exchange{A, A, A, A, 0.1f};

GlobalSettings& settings = GlobalSettings::get();

void insertDefaultReactions(GlobalSettings& globalSettings)
{
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

    const DiscType::map<int> distribution{{A, 100}};
    EXPECT_NO_THROW(settings.setDiscTypeDistribution(distribution));

    EXPECT_NO_THROW(settings.addReaction(decomposition));
    EXPECT_NO_THROW(settings.clearReactions());

    // Lock the settings so that further changes are prohibited.
    settings.lock();

    // Now all modification calls should throw std::runtime_error.
    EXPECT_THROW(settings.setSimulationTimeStep(SettingsLimits::MinSimulationTimeStep), std::runtime_error);
    EXPECT_THROW(settings.setSimulationTimeScale(SettingsLimits::MinSimulationTimeScale), std::runtime_error);
    EXPECT_THROW(settings.setNumberOfDiscs(SettingsLimits::MinNumberOfDiscs), std::runtime_error);
    EXPECT_THROW(settings.setFrictionCoefficient(SettingsLimits::MinFrictionCoefficient), std::runtime_error);
    EXPECT_THROW(settings.setDiscTypeDistribution(distribution), std::runtime_error);
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
        DiscType::map<int> distribution{{A, i}};

        if (i == 100)
            EXPECT_NO_THROW(settings.setDiscTypeDistribution(distribution));
        else
            EXPECT_ANY_THROW(settings.setDiscTypeDistribution(distribution));
    }
}

TEST(GlobalSettingsTest, DuplicateNamesInDistributionArentAllowed)
{
    DiscType::map<int> distribution{{A, 50}, {ACopy, 50}};

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
    settings.setDiscTypeDistribution({{A, 50}, {B, 50}});

    Reaction noDuplicateCombination{A, B, A, std::nullopt, 0.1f};
    Reaction noDuplicateExchange{A, B, A, B, 0.1f};

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
    DiscType::map<int> distribution{{A, 100}, {B, 0}, {C, 0}};
    settings.setDiscTypeDistribution(distribution);

    // 1 reaction with A as educt, 1 with A as product, 1 with A nowhere, for each reaction type
    Reaction decompositionAEduct{A, std::nullopt, C, B, 0.1f};
    Reaction decompositionAProduct{B, std::nullopt, A, B, 0.1f};
    Reaction decompositionANowhere{B, std::nullopt, C, B, 0.1f};

    Reaction combinationAEduct{A, B, C, std::nullopt, 0.1f};
    Reaction combinationAProduct{B, C, A, std::nullopt, 0.1f};
    Reaction combinationANowhere{B, C, C, std::nullopt, 0.1f};

    Reaction exchangeAEduct{A, B, C, B, 0.1f};
    Reaction exchangeAProduct{B, C, A, A, 0.1f};
    Reaction exchangeANowhere{B, C, C, C, 0.1f};

    settings.clearReactions();

    // Add all these reactions
    std::vector<Reaction> reactions = {decompositionAEduct, decompositionAProduct, decompositionANowhere,
                                       combinationAEduct,   combinationAProduct,   combinationANowhere,
                                       exchangeAEduct,      exchangeAProduct,      exchangeANowhere};

    for (const auto& reaction : reactions)
        settings.addReaction(reaction);

    // Make sure they were all added (some twice because educts were not identical for combination/exchange reactions)

    EXPECT_EQ(settings.getSettings().decompositionReactions_.size(), 2);
    EXPECT_EQ(settings.getSettings().combinationReactions_.size(), 4);
    EXPECT_EQ(settings.getSettings().exchangeReactions_.size(), 4);

    EXPECT_EQ(settings.getSettings().decompositionReactions_.at(A).size(), 1);
    EXPECT_EQ(settings.getSettings().decompositionReactions_.at(B).size(), 2);

    const auto& eductsAB1 = std::make_pair(A, B);
    const auto& eductsAB2 = std::make_pair(B, A);
    const auto& eductsBC1 = std::make_pair(B, C);
    const auto& eductsBC2 = std::make_pair(C, B);

    EXPECT_EQ(settings.getSettings().combinationReactions_.at(eductsAB1).size(), 1);
    EXPECT_EQ(settings.getSettings().combinationReactions_.at(eductsAB2).size(), 1);
    EXPECT_EQ(settings.getSettings().combinationReactions_.at(eductsBC1).size(), 2);
    EXPECT_EQ(settings.getSettings().combinationReactions_.at(eductsBC2).size(), 2);

    EXPECT_EQ(settings.getSettings().exchangeReactions_.at(eductsAB1).size(), 1);
    EXPECT_EQ(settings.getSettings().exchangeReactions_.at(eductsAB2).size(), 1);
    EXPECT_EQ(settings.getSettings().exchangeReactions_.at(eductsBC1).size(), 2);
    EXPECT_EQ(settings.getSettings().exchangeReactions_.at(eductsBC2).size(), 2);

    // Now remove A from the distribution
    distribution = {{B, 100}, {C, 0}};

    settings.setDiscTypeDistribution(distribution);

    EXPECT_EQ(settings.getSettings().decompositionReactions_.size(), 1);
    EXPECT_FALSE(isInEductsOrProducts(settings.getSettings().decompositionReactions_, A));

    EXPECT_EQ(settings.getSettings().combinationReactions_.size(), 2);
    EXPECT_FALSE(isInEductsOrProducts(settings.getSettings().combinationReactions_, A));

    EXPECT_EQ(settings.getSettings().exchangeReactions_.size(), 2);
    EXPECT_FALSE(isInEductsOrProducts(settings.getSettings().exchangeReactions_, A));
}

TEST(GlobalSettingsTest, DiscTypesInReactionsAreUpdated)
{
    DiscType::map<int> distribution{{A, 100}};

    settings.setDiscTypeDistribution(distribution);
    settings.clearReactions();

    settings.addReaction(Reaction{A, std::nullopt, A, A, 0.1f});
    settings.addReaction(Reaction{A, A, A, std::nullopt, 0.1f});
    settings.addReaction(Reaction{A, A, A, A, 0.1f});

    int count = countDiscTypeInReactions(settings, A);

    DiscType AModified = A;
    AModified.setName("AModified");
    distribution = {{AModified, 100}};
    settings.setDiscTypeDistribution(distribution);

    EXPECT_EQ(count, countDiscTypeInReactions(settings, AModified));
}

TEST(GlobalSettingsTest, CantAddReactionsWithDiscTypesThatArentInDistribution)
{
    settings.setDiscTypeDistribution({{A, 100}});
    settings.clearReactions();

    EXPECT_ANY_THROW(settings.addReaction(Reaction{B, std::nullopt, B, B, 0.1f}));
    EXPECT_ANY_THROW(settings.addReaction(Reaction{B, B, B, std::nullopt, 0.1f}));
    EXPECT_ANY_THROW(settings.addReaction(Reaction{B, B, B, B, 0.1f}));
    EXPECT_NO_THROW(settings.addReaction(Reaction{A, A, A, A, 0.1f}));
}

TEST(GlobalSettingsTest, EmptyReactionVectorsAreRemoved)
{
    settings.setDiscTypeDistribution({{A, 100}});
    settings.clearReactions();

    settings.addReaction(Reaction{A, std::nullopt, A, A, 0.1f});
    settings.addReaction(Reaction{A, A, A, std::nullopt, 0.1f});
    settings.addReaction(Reaction{A, A, A, A, 0.1f});

    settings.setDiscTypeDistribution({{B, 100}});

    EXPECT_TRUE(settings.getSettings().decompositionReactions_.empty());
    EXPECT_TRUE(settings.getSettings().combinationReactions_.empty());
    EXPECT_TRUE(settings.getSettings().exchangeReactions_.empty());
}