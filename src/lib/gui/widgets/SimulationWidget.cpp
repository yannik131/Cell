#include "widgets/SimulationWidget.hpp"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <glog/logging.h>

#include <map>

SimulationWidget::SimulationWidget(QWidget* parent)
    : QSFMLWidget(parent)
{
    boundingRect_.setOutlineColor(sf::Color::Yellow);
    boundingRect_.setOutlineThickness(1);
    boundingRect_.setFillColor(sf::Color::Transparent);
}

void SimulationWidget::render(const FrameDTO& frameDTO)
{
    // The settings are only locked if the simulation is running
    // If it is not running, we have no frame limit
    clock_.restart();

    sf::RenderWindow::clear(sf::Color::Black);

    for (const auto& disc : frameDTO.discs_)
    {
        sf::CircleShape circleShape;
        sf::RenderWindow::draw(circleShape);
    }

    sf::RenderWindow::draw(boundingRect_);
    sf::RenderWindow::display();
}
