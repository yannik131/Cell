#include "cell/SimulationConfigBuilder.hpp"

#include <gmock/gmock.h>
#include <nlohmann/json.hpp>

#include <fstream>

using namespace cell;
using namespace testing;
using json = nlohmann::json;

TEST(ASimulationConfig, CanBeSerialized)
{
    SimulationConfigBuilder builder;
    builder.addDiscType("A", Radius{1}, Mass{1});
    builder.addDiscType("B", Radius{1}, Mass{1});
    builder.addDiscType("C", Radius{2}, Mass{2});

    builder.addReaction("A", "", "B", "", Probability{0.5});
    builder.addReaction("A", "B", "C", "", Probability{1});
    builder.addReaction("C", "", "A", "B", Probability{0.5});
    builder.addReaction("A", "C", "B", "C", Probability{0.5});

    builder.addMembraneType("Large", Radius{200}, {});
    builder.addMembrane("Large", Position{.x = 500, .y = 500});

    builder.useDistribution(true);
    builder.setDistribution("", {{"A", 0.5}, {"B", 0.5}});
    builder.setDistribution("Large", {{"A", 1}, {"B", 0}});
    builder.addDisc("C", Position{.x = 50, .y = 50}, Velocity{.x = 50, .y = 50});
    builder.setCellDimensions(Width{100}, Height{200});
    builder.setDiscCount(10);
    builder.setTimeScale(0.5);
    builder.setTimeStep(1e-3);
    builder.setMaxVelocity(100);

    auto config = builder.getSimulationConfig();

    {
        json j = config;
        std::ofstream out("config.json");
        out << j.dump(4);
    }

    {
        json j;
        std::ifstream in("config.json");
        in >> j;
        auto readConfig = j.get<SimulationConfig>();
        ASSERT_THAT(config, Eq(readConfig));
    }
}