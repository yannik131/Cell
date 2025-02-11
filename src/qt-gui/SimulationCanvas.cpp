#include "SimulationCanvas.hpp"

#include <glog/logging.h>

SimulationCanvas::SimulationCanvas(QWidget* parent) : QSFMLCanvas(parent), world_(*this)
{
    font_.loadFromFile("../../resources/Sansation.ttf");
    statisticsText_.setFont(font_);
    statisticsText_.setPosition(5.f, 5.f);
    statisticsText_.setCharacterSize(10);
}

int SimulationCanvas::getAndResetCollisions()
{
    return world_.getAndResetCollisionCount();
}

void SimulationCanvas::onInit()
{
}

void SimulationCanvas::onUpdate()
{
    if(timePerFrame_ > TimePerFrameTarget)
        timePerFrame_ -= sf::microseconds(5);
    
    sf::Time elapsedTime = clock_.restart();
    timeSinceLastUpdate += elapsedTime;

    while(timeSinceLastUpdate > timePerFrame_) {
        timeSinceLastUpdate -= timePerFrame_;
        world_.update(timePerFrame_);
    }

    updateStatisticsText(elapsedTime);
    render();
}

void SimulationCanvas::render()
{
    sf::RenderWindow::clear(sf::Color::Black);
    world_.draw();
    sf::RenderWindow::draw(statisticsText_);
}

void SimulationCanvas::updateStatisticsText(const sf::Time& dt)
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
