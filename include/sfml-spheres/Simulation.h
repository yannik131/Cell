#ifndef SIMULATION_H
#define SIMULATION_H

#include <SFML/System/NonCopyable.hpp>

class Simulation : private sf::NonCopyable 
{
public:
    Simulation() = default;
    
    void run() {}
};

#endif /* SIMULATION_H */
