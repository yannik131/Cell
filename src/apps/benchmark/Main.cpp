// configure using cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo .. and profile with VerySleepy

#include "cell/Cell.hpp"
#include "cell/Disc.hpp"
#include "cell/SimulationConfigBuilder.hpp"
#include "cell/SimulationFactory.hpp"
#include "cell/StringUtils.hpp"
#include "cell/Types.hpp"

#include <chrono>
#include <iostream>

using namespace cell;

int main()
{
    SimulationConfigBuilder builder;

    builder.addMembraneType(
        "M", Radius{500},
        {{"A", MembraneType::Permeability::Inward}, {"B", MembraneType::Permeability::Bidirectional}});

    std::vector<Vector2d> membranePositions;
    for (int i = 0; i < 5; ++i)
        builder.addMembrane("M", Position{.x = -3000.0 + i * 1000, .y = -3000.0 + i * 1000});

    builder.setCellMembraneType(Radius{5000}, {});
    builder.addDiscType("A", Radius{10}, Mass{5});
    builder.addDiscType("B", Radius{10}, Mass{5});
    builder.addDiscType("C", Radius{12}, Mass{10});

    builder.setDistribution("", {{"A", 1}});
    builder.setDistribution("M", {{"A", 1}});

    builder.setDiscCount("", 20000);
    builder.setDiscCount("M", 1000);
    builder.useDistribution(true);

    builder.addReaction("A", "", "B", "", Probability{0.1});
    builder.addReaction("A", "B", "C", "", Probability{0.1});
    builder.addReaction("B", "C", "A", "C", Probability{0.2});
    builder.addReaction("C", "", "A", "B", Probability{0.1});

    SimulationFactory simulationContext;
    simulationContext.buildSimulationFromConfig(builder.getSimulationConfig());

    auto& cell = simulationContext.getCell();
    const auto& registry = simulationContext.getSimulationContext().discTypeRegistry;

    using clock = std::chrono::steady_clock;
    using namespace std::chrono_literals;

    std::cout << "Starting benchmark" << "\n";
    auto start = clock::now();

    int N = 0;
    while ((clock::now() - start) < 10s)
    {
        cell.update(1e-3);
        ++N;
    }

    auto end = clock::now();

    std::cout << "Done" << "\n";
    long long ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    std::cout << "Finished updates: " << N << "\n";
    std::cout << "Elapsed time: " << cell::stringutils::timeString(ns) << "\n";
    std::cout << "Time per update: " << cell::stringutils::timeString(ns / N) << "\n";

    for (const auto& [typeID, count] : CollisionDetector::getAndResetCollisionCounts())
        std::cout << registry.getByID(typeID).getName() << ": " << count << " collisions" << "\n";
}