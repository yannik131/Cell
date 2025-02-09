#include "Simulation.hpp"

#include <SFML/System/Sleep.hpp>
#include <SFML/Window/Event.hpp>

#include <glog/logging.h>

const sf::Time Simulation::TimePerFrame = sf::milliseconds(1000.f / 100);

Simulation::Simulation(sf::RenderWindow& renderWindow) : renderWindow_(renderWindow), world_(renderWindow_)
{
    font_.loadFromFile("../../resources/Sansation.ttf");
    statisticsText_.setFont(font_);
    statisticsText_.setPosition(5.f, 5.f);
    statisticsText_.setCharacterSize(10);
}

void Simulation::update(const sf::Time& dt)
{
    world_.update(dt);
}

void Simulation::processEvents()
{
    sf::Event event;
    while(renderWindow_.pollEvent(event)) {
        if(event.type == sf::Event::Closed || 
        (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
            renderWindow_.close();
    }
}

void Simulation::render()
{
    renderWindow_.clear(sf::Color::Black);
    world_.draw();
    renderWindow_.draw(statisticsText_);
}

void Simulation::updateStatisticsText(const sf::Time& dt)
{
    statisticsUpdateTime_ += dt;
    statisticsFrameCount_++;
    
    if(statisticsUpdateTime_ < sf::seconds(1.f))
        return;
        
    statisticsText_.setString(
        "Frames/s: " + std::to_string(statisticsFrameCount_) + "\n" + 
        "Time/Update: " + std::to_string(statisticsUpdateTime_.asMilliseconds() / statisticsFrameCount_) + "ms"
    );
    
    statisticsUpdateTime_ -= sf::seconds(1.f);
    statisticsFrameCount_ = 0;
}
