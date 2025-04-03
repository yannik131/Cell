#include "SimulationWidget.hpp"

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
    sf::CircleShape shape(disc.getType().radius_);
    shape.setFillColor(disc.getType().color_);
    shape.setOrigin(sf::Vector2f(disc.getType().radius_, disc.getType().radius_));
    shape.setPosition(disc.getPosition());

    return shape;
}
