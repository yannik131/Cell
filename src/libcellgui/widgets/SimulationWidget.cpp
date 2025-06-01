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
}

void SimulationWidget::initialize(const std::vector<Disc>& discs)
{
    render({.discs_ = discs});
}

void SimulationWidget::render(const FrameDTO& frameDTO)
{
    // The settings are only locked if the simulation is running
    // If it is not running, we have no frame limit
    if (GlobalSettings::get().isLocked() &&
        clock_.getElapsedTime() < sf::seconds(1.f / static_cast<float>(GlobalGUISettings::getGUISettings().guiFPS_)))
        return;

    clock_.restart();

    sf::RenderWindow::clear(sf::Color::Black);

    for (const auto& disc : frameDTO.discs_)
    {
        sf::CircleShape circleShape = circleShapeFromDisc(disc);
        sf::RenderWindow::draw(circleShape);
    }

    sf::RenderWindow::display();
}

sf::CircleShape SimulationWidget::circleShapeFromDisc(const Disc& disc)
{
    sf::CircleShape shape(disc.getType().getRadius());
    shape.setFillColor(disc.getType().getColor());
    shape.setOrigin(sf::Vector2f(disc.getType().getRadius(), disc.getType().getRadius()));
    shape.setPosition(disc.getPosition());

    return shape;
}
