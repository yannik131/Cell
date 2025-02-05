#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include "World.hpp"

#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>

class Simulation : private sf::NonCopyable 
{
public:
    Simulation();
    
    void run();
    
private:
    void processEvents();
    void render();
    
private:
    static const sf::Time TimePerFrame;
    
    sf::Clock clock_;
    sf::RenderWindow renderWindow_;
    World world_;
};

#endif /* SIMULATION_HPP */
