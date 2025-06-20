// configure using cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo .. and profile with VerySleepy

#include "Cell.hpp"
#include "GlobalSettings.hpp"
#include "Logging.hpp"
#include "StringUtils.hpp"

#include <chrono>

#include <glog/logging.h>

void setBenchmarkSettings()
{
    cell::GlobalSettings::get().setNumberOfDiscs(800);
}

int main(int argc, char** argv)
{
    cell::initLogging(argc, argv);
    setBenchmarkSettings();

    cell::Cell world;
    cell::GlobalSettings::get().setCellSize(1000, 1000);
    world.reinitialize();

    const int N = 100000;
    LOG(INFO) << "Starting benchmark";
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < N; ++i)
        world.update(sf::milliseconds(1));

    auto end = std::chrono::high_resolution_clock::now();

    LOG(INFO) << "Done";
    long long ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    LOG(INFO) << "Elapsed time: " << cell::stringutils::timeString(ns);
    LOG(INFO) << "Time per update: " << cell::stringutils::timeString(ns / N);
}