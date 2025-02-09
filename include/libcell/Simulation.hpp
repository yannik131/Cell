#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include "World.hpp"

#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>

#include <memory>

class Simulation : private sf::NonCopyable 
{
public:
    explicit Simulation(sf::RenderWindow& renderWindow);
    
    void update(const sf::Time& dt);
    void updateStatisticsText(const sf::Time& dt);
    
    void processEvents();
    void render();
    
private:
    static const sf::Time TimePerFrame;
    
    sf::RenderWindow& renderWindow_;
    World world_;
    sf::Font font_;
    sf::Text statisticsText_;
    
    sf::Time statisticsUpdateTime_ = sf::Time::Zero;
    int statisticsFrameCount_ = 0;
};

#endif /* SIMULATION_HPP */
