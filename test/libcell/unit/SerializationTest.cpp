// Using nlohmann::json to serialize the settings turned out to be a little challenging, so this test suite makes sure
// that each individual class can be properly serialized

#include "DiscType.hpp"
#include "GlobalSettings.hpp"
#include "Reaction.hpp"
#include "ReactionTable.hpp"
#include "Settings.hpp"
#include "TestUtils.hpp"

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include <fstream>

using namespace cell;
using json = nlohmann::json;

TEST(SerializationTest, DiscTypeSerialization)
{
    DiscType A{"A", sf::Color::Blue, 3.5f, 1.5f};

    {
        json j = A;
        std::ofstream out("A.json");
        out << j.dump(4);
    }

    {
        json j;
        std::ifstream in("A.json");
        in >> j;
        EXPECT_EQ(A, j.get<DiscType>());
    }
}

TEST(SerializationTest, ReactionSerialization)
{
    cell::Reaction transformation, decomposition, combination, exchange;
    std::tie(transformation, decomposition, combination, exchange) = getDefaultReactions();
    std::vector<cell::Reaction> reactions({transformation, decomposition, combination, exchange});

    {
        json j = reactions;
        std::ofstream out("reactions.json");
        out << j.dump(4);
    }

    {
        json j;
        std::ifstream in("reactions.json");
        in >> j;
        auto reactionsRead = j.get<std::vector<cell::Reaction>>();

        EXPECT_EQ(reactionsRead.size(), reactions.size());

        for (std::size_t i = 0; i < reactions.size(); ++i)
            EXPECT_EQ(reactions[i], reactionsRead[i]);
    }
}

TEST(SerializationTest, ReactionTableSerialization)
{
    cell::Reaction transformation, decomposition, combination, exchange;
    std::tie(transformation, decomposition, combination, exchange) = getDefaultReactions();

    cell::ReactionTable reactionTable;
    reactionTable.setReactions({transformation, decomposition, combination, exchange});

    {
        json j = reactionTable;
        std::ofstream out("reactionTable.json");
        out << j.dump(4);
    }

    {
        json j;
        std::ifstream in("reactionTable.json");
        in >> j;
        auto reactionTableRead = j.get<cell::ReactionTable>();

        EXPECT_EQ(reactionTable.getTransformationReactionLookupMap(),
                  reactionTableRead.getTransformationReactionLookupMap());
        EXPECT_EQ(reactionTable.getDecompositionReactionLookupMap(),
                  reactionTableRead.getDecompositionReactionLookupMap());
        EXPECT_EQ(reactionTable.getCombinationReactionLookupMap(), reactionTableRead.getCombinationReactionLookupMap());
        EXPECT_EQ(reactionTable.getExchangeReactionLookupMap(), reactionTableRead.getExchangeReactionLookupMap());
    }
}

TEST(SerializationTest, SettingsSerialization)
{
    DiscType A("A", sf::Color::Green, 5, 10);
    DiscType B("B", sf::Color::Red, 10, 5);
    DiscType C("C", sf::Color::Blue, 12, 5);
    DiscType D("D", sf::Color::Magenta, 15, 10);

    DiscType::map<int> discTypeDistribution;

    discTypeDistribution[A] = 100;
    discTypeDistribution[B] = 0;
    discTypeDistribution[C] = 0;
    discTypeDistribution[D] = 0;

    GlobalSettings::get().setDiscTypeDistribution(discTypeDistribution);
    GlobalSettings::get().setNumberOfDiscs(100);
    GlobalSettings::get().setSimulationTimeScale(1);
    GlobalSettings::get().setSimulationTimeStep(sf::milliseconds(1));

    GlobalSettings::get().addReaction(Reaction{A, std::nullopt, B, C, 1e-2f});
    GlobalSettings::get().addReaction(Reaction{B, C, D, std::nullopt, 1e-2f});
    GlobalSettings::get().addReaction(Reaction{B, A, C, D, 1e-2f});
    auto settings = GlobalSettings::getSettings();

    {
        json j = settings;
        std::ofstream out("settings.json");
        out << j.dump(4);
    }

    {
        json j;
        std::ifstream in("settings.json");
        in >> j;
        auto settingsRead = j.get<cell::Settings>();

        EXPECT_EQ(settings.discTypeDistribution_, settingsRead.discTypeDistribution_);
        EXPECT_EQ(settings.numberOfDiscs_, settingsRead.numberOfDiscs_);
        EXPECT_EQ(settings.simulationTimeScale_, settingsRead.simulationTimeScale_);
        EXPECT_EQ(settings.simulationTimeStep_, settingsRead.simulationTimeStep_);
    }
}