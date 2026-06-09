#include <chrono>
#include <iostream>
#include <thread>

namespace ch = std::chrono;
using namespace std::chrono_literals;

int main()
{
    const ch::duration<double> simulationTimeStep = 1ms;
    const double targetScale = 0.01;
    const ch::duration<double> realTimePerStep = simulationTimeStep / targetScale;
    auto next = ch::steady_clock::now();
    std::cout << "Target scale: " << targetScale << "\n";

    for (int frame = 1; frame <= 10; ++frame)
    {
        const auto start = ch::steady_clock::now();
        std::cout << "New frame: " << frame << "\n";
        next += ch::duration_cast<ch::steady_clock::duration>(realTimePerStep);
        std::this_thread::sleep_until(next);
        const auto elapsed = ch::steady_clock::now() - start;
        std::cout << "Elapsed time: " << ch::duration<double>(elapsed).count() << "s\n";
        std::cout << "Actual scale: " << simulationTimeStep / elapsed << "\n";
    }

    return 0;
}