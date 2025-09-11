#include "widgets/SimulationWidget.hpp"
#include "core/AbstractSimulationBuilder.hpp"

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

void SimulationWidget::render(const std::vector<cell::Disc>& discs, const cell::DiscTypeResolver& discTypeResolver,
                              const std::map<std::string, sf::Color>& colorMap)
{
    sf::RenderWindow::clear(sf::Color::Black);

    for (const auto& disc : discs)
    {
        const auto& discType = discTypeResolver(disc.getDiscTypeID());

        sf::CircleShape circleShape;
        circleShape.setPosition(static_cast<sf::Vector2f>(disc.getPosition()));
        circleShape.setRadius(discType.getRadius());
        circleShape.setFillColor(colorMap.at(discType.getName()));

        sf::RenderWindow::draw(circleShape);
    }

    sf::RenderWindow::draw(boundingRect_);
    sf::RenderWindow::display();
}
