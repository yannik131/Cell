#include "Simulation.hpp"

#include <SFML/System/Sleep.hpp>
#include <SFML/Window/Event.hpp>

#include <glog/logging.h>

const sf::Time Simulation::TimePerFrame = sf::milliseconds(1000.f / 100);

Simulation::Simulation() : renderWindow_(sf::VideoMode(2560, 1440), "Balls", sf::Style::Fullscreen), world_(renderWindow_)
{
    font_.loadFromFile("../resources/Sansation.ttf");
    statisticsText_.setFont(font_);
    statisticsText_.setPosition(5.f, 5.f);
    statisticsText_.setCharacterSize(10);
}

void Simulation::run()
{
    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    
    while(renderWindow_.isOpen()) {
        sf::Time elapsedTime = clock.restart();
        timeSinceLastUpdate += elapsedTime;

        while(timeSinceLastUpdate > TimePerFrame) {
            timeSinceLastUpdate -= TimePerFrame;
            
            processEvents();
            world_.update(TimePerFrame);
        }
        
        updateStatisticsText(elapsedTime);
        render();
    }
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
    renderWindow_.display();
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
