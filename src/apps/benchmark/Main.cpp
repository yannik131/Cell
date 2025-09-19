// configure using cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo .. and profile with VerySleepy

#include "cell/Disc.hpp"
#include "cell/Logging.hpp"
#include "cell/SimulationConfigBuilder.hpp"
#include "cell/SimulationContext.hpp"
#include "cell/StringUtils.hpp"
#include "cell/Types.hpp"

#include <chrono>

#include <glog/logging.h>

using namespace cell;

int main(int argc, char** argv)
{
    initLogging(argc, argv);

    SimulationConfigBuilder builder;

    builder.setCellDimensions(Width{1000.0}, Height{1000.0});
    builder.setTimeStep(1e-3);
    builder.setDiscCount(800);
    builder.useDistribution(true);

    builder.addDiscType("A", Radius{10}, Mass{5});
    builder.addDiscType("B", Radius{10}, Mass{5});
    builder.addDiscType("C", Radius{12}, Mass{10});

    builder.setDistribution({{"A", 1}});

    builder.addReaction("A", "", "B", "", Probability{0.1});
    builder.addReaction("A", "B", "C", "", Probability{0.1});
    builder.addReaction("B", "C", "A", "C", Probability{0.2});
    builder.addReaction("C", "", "A", "B", Probability{0.1});

    SimulationContext simulationContext;
    simulationContext.buildContextFromConfig(builder.getSimulationConfig());

    auto& cell = simulationContext.getCell();
    auto discTypeResolver = simulationContext.getDiscTypeRegistry().getDiscTypeResolver();

    const int N = 100000;
    LOG(INFO) << "Starting benchmark";
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < N; ++i)
        cell.update();

    auto end = std::chrono::high_resolution_clock::now();

    LOG(INFO) << "Done";
    long long ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    LOG(INFO) << "Elapsed time: " << cell::stringutils::timeString(ns);
    LOG(INFO) << "Time per update: " << cell::stringutils::timeString(ns / N);

    for (const auto& [typeID, count] : simulationContext.getAndResetCollisionCounts())
        LOG(INFO) << discTypeResolver(typeID).getName() << ": " << count << " collisions";
}