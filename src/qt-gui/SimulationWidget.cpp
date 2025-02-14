#include "SimulationWidget.hpp"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <glog/logging.h>

#include <map>

SimulationWidget::SimulationWidget(QWidget* parent) : QSFMLWidget(parent)
{
}

void SimulationWidget::render(const FrameDTO& frameDTO)
{
    sf::RenderWindow::clear(sf::Color::Black);

    for(const auto& guiDisc : frameDTO.discs_) {
        auto& circleShape = circles_[guiDisc.index_];
        circleShape.setPosition(guiDisc.position_);

        sf::RenderWindow::draw(circleShape);
    }

    sf::RenderWindow::display();
}

void SimulationWidget::initialize(const std::vector<Disc>& discs)
{
    const std::map<int, sf::Color> Colors = {{5, sf::Color::Green}, {10, sf::Color::Red}, {12, sf::Color::Blue}, {15, sf::Color::Yellow}};

    if(circles_.empty())
        circles_.reserve(discs.size());
    else 
        circles_.clear();

    for(const auto& disc : discs) {
        sf::CircleShape shape(disc.radius_);
        shape.setFillColor(Colors.at(disc.radius_));
        shape.setOrigin(sf::Vector2f(disc.radius_, disc.radius_));
        shape.setPosition(disc.position_);

        circles_.push_back(shape);
    }
}