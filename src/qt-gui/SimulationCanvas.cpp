#include "SimulationCanvas.hpp"

#include <glog/logging.h>

SimulationCanvas::SimulationCanvas(QWidget* parent) : QSFMLCanvas(parent), simulation_(*this)
{
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
        simulation_.processEvents();
        simulation_.update(timePerFrame_);
    }

    simulation_.updateStatisticsText(elapsedTime);
    simulation_.render();
}
