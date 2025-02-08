#include "Simulation.hpp"
#include "Logging.hpp"

#include <glog/logging.h>

#include <exception>

int main(int argc, char** argv) {
    initLogging(argc, argv);
    
    Simulation simulation;
    
    try {
        simulation.run();
    }
    catch(const std::exception& e) {
        LOG(ERROR) << "Unhandled exception occured: " << e.what();
        return 1;
    }
    catch(...) {
        LOG(ERROR) << "Unknown exception occured.";
        return 1;
    }
    
    return 0;
}