// configure using cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo .. and profile with VerySleepy

#include "cell/Cell.hpp"
#include "cell/Disc.hpp"
#include "cell/Logging.hpp"
#include "cell/SimulationConfigBuilder.hpp"
#include "cell/SimulationFactory.hpp"
#include "cell/StringUtils.hpp"
#include "cell/Types.hpp"

#include <chrono>

#include <glog/logging.h>

using namespace cell;

int main(int argc, char** argv)
{
    initLogging(argc, argv);

    SimulationConfigBuilder builder;

    builder.addMembraneType(
        "M", Radius{100},
        {{"A", MembraneType::Permeability::Inward}, {"B", MembraneType::Permeability::Bidirectional}});
    builder.addMembrane("M", Position{.x = 500, .y = 500});

    builder.setCellMembraneType(Radius{1000}, {});
    builder.addDiscType("A", Radius{10}, Mass{5});
    builder.addDiscType("B", Radius{10}, Mass{5});
    builder.addDiscType("C", Radius{12}, Mass{10});

    builder.setDistribution("", {{"A", 1}});
    builder.setDistribution("M", {{"A", 1}});

    builder.setDiscCount("", 800);
    builder.setDiscCount("M", 30);
    builder.useDistribution(true);

    builder.addReaction("A", "", "B", "", Probability{0.1});
    builder.addReaction("A", "B", "C", "", Probability{0.1});
    builder.addReaction("B", "C", "A", "C", Probability{0.2});
    builder.addReaction("C", "", "A", "B", Probability{0.1});

    SimulationFactory simulationContext;
    simulationContext.buildSimulationFromConfig(builder.getSimulationConfig());

    auto& cell = simulationContext.getCell();
    const auto& registry = simulationContext.getSimulationContext().discTypeRegistry;

    const int N = 100000;
    LOG(INFO) << "Starting benchmark";
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < N; ++i)
        cell.update(1e-3);

    auto end = std::chrono::high_resolution_clock::now();

    LOG(INFO) << "Done";
    long long ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    LOG(INFO) << "Elapsed time: " << cell::stringutils::timeString(ns);
    LOG(INFO) << "Time per update: " << cell::stringutils::timeString(ns / N);

    for (const auto& [typeID, count] : simulationContext.getAndResetCollisionCounts())
        LOG(INFO) << registry.getByID(typeID).getName() << ": " << count << " collisions";
}