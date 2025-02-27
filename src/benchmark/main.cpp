#include "GlobalSettings.hpp"
#include "Logging.hpp"
#include "StringUtils.hpp"
#include "World.hpp"

#include <chrono>

#include <glog/logging.h>

void setBenchmarkSettings()
{
    GlobalSettings::get().setNumberOfDiscs(400);
}

int main(int argc, char** argv)
{
    initLogging(argc, argv);

    World world;
    world.setBounds(sf::Vector2f(600, 600));
    world.reinitialize();

    LOG(INFO) << "Starting benchmark";
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 1000; ++i)
        world.update(sf::milliseconds(5));

    auto end = std::chrono::high_resolution_clock::now();

    LOG(INFO) << "Done";
    long long ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    LOG(INFO) << "Elapsed time: " << StringUtils::timeString(ns);
    LOG(INFO) << "Time per update: " << StringUtils::timeString(ns / 1e3);
}