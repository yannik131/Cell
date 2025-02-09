#include "SimulationCanvas.hpp"

#include <glog/logging.h>

SimulationCanvas::SimulationCanvas(QWidget* parent) : QSFMLCanvas(parent), simulation_(*this)
{
}

void SimulationCanvas::onInit()
{
    //setSize(sf::Vector2u(QWidget::width(), QWidget::height()));
}

void SimulationCanvas::onUpdate()
{
    const sf::Time TimePerFrame = sf::milliseconds(1000.f / 60);

    sf::Time elapsedTime = clock_.restart();
    timeSinceLastUpdate += elapsedTime;

    while(timeSinceLastUpdate > TimePerFrame) {
        timeSinceLastUpdate -= TimePerFrame;
        simulation_.processEvents();
        simulation_.update(TimePerFrame);
    }

    simulation_.updateStatisticsText(elapsedTime);
    simulation_.render();
}
