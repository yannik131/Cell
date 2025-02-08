#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include "World.hpp"

#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>

class Simulation : private sf::NonCopyable 
{
public:
    Simulation();
    
    void run();
    
private:
    void processEvents();
    void render();
    void updateStatisticsText(const sf::Time& dt);
    
private:
    static const sf::Time TimePerFrame;
    
    sf::RenderWindow renderWindow_;
    World world_;
    sf::Font font_;
    sf::Text statisticsText_;
    
    sf::Time statisticsUpdateTime_ = sf::Time::Zero;
    int statisticsFrameCount_ = 0;
};

#endif /* SIMULATION_HPP */
