#include "Simulation.hpp"

#include <SFML/System/Sleep.hpp>
#include <SFML/Window/Event.hpp>

#include <glog/logging.h>

const sf::Time Simulation::TimePerFrame = sf::milliseconds(1000.f / 60);

Simulation::Simulation() : renderWindow_(sf::VideoMode(1000, 1000), "Balls", sf::Style::Close), world_(renderWindow_)
{
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
    renderWindow_.display();
}
