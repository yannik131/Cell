#include "World.hpp"
#include "Logging.hpp"

#include <chrono>
#include <string>

#include <glog/logging.h>

std::string time_string(long long time_ns)
{
    std::vector<std::string> time_units = {"ns", "us", "ms", "s"};

    int i = 0;
    double converted_time = time_ns;
    while (converted_time > 1e3 && i < time_units.size())
    {
        converted_time /= 1e3;
        ++i;
    }

    return std::to_string(converted_time) + time_units[i];
}

int main(int argc, char** argv)
{
    initLogging(argc, argv);

    World world;
    world.setNumberOfDiscs(400);
    world.setBounds(sf::Vector2f(600, 600));
    world.reinitialize();

    LOG(INFO) << "Starting benchmark";
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 1000; ++i)
        world.update(sf::milliseconds(5));

    auto end = std::chrono::high_resolution_clock::now();

    LOG(INFO) << "Done";
    long long ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    LOG(INFO) << "Elapsed time: " << time_string(ns);
    LOG(INFO) << "Time per update: " << time_string(ns / 1e3);
}