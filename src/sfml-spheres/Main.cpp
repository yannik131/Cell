#include "Simulation.h"

#include <glog/logging.h>

#include <exception>

int main() {
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