#include "SimulationWidget.hpp"
#include "GlobalGUISettings.hpp"
#include "GlobalSettings.hpp"

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
    if (cell::GlobalSettings::get().isLocked() &&
        clock_.getElapsedTime() < sf::seconds(1.0f / static_cast<float>(GlobalGUISettings::getGUISettings().guiFPS_)))
        return;

    clock_.restart();

    sf::RenderWindow::clear(sf::Color::Black);

    for (const auto& disc : frameDTO.discs_)
    {
        sf::CircleShape circleShape = circleShapeFromDisc(disc);
        sf::RenderWindow::draw(circleShape);
    }

    boundingRect_.setSize(sf::Vector2f{static_cast<float>(cell::GlobalSettings::getSettings().cellWidth_),
                                       static_cast<float>(cell::GlobalSettings::getSettings().cellHeight_)});
    boundingRect_.setOutlineThickness(static_cast<float>(QSFMLWidget::getCurrentZoom()));

    sf::RenderWindow::draw(boundingRect_);
    sf::RenderWindow::display();
}

sf::CircleShape SimulationWidget::circleShapeFromDisc(const cell::Disc& disc)
{
    sf::CircleShape shape(static_cast<float>(disc.getType()->getRadius()));
    shape.setFillColor(disc.getType()->getColor());
    auto radius = static_cast<float>(disc.getType()->getRadius());
    shape.setOrigin(sf::Vector2f(radius, radius));
    shape.setPosition(static_cast<sf::Vector2f>(disc.getPosition()));

    return shape;
}
