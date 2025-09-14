#include "widgets/SimulationWidget.hpp"
#include "core/AbstractSimulationBuilder.hpp"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <glog/logging.h>

#include "SimulationWidget.hpp"
#include <map>

SimulationWidget::SimulationWidget(QWidget* parent)
    : QSFMLWidget(parent)
{
    boundingRect_.setOutlineColor(sf::Color::Yellow);
    boundingRect_.setOutlineThickness(1);
    boundingRect_.setFillColor(sf::Color::Transparent);
}

void SimulationWidget::injectAbstractSimulationBuilder(AbstractSimulationBuilder* abstractSimulationBuilder)
{
    abstractSimulationBuilder_ = abstractSimulationBuilder;
    abstractSimulationBuilder->registerConfigObserver(
        [&](const cell::SimulationConfig& config, const std::map<std::string, sf::Color>&)
        {
            boundingRect_.setSize(
                sf::Vector2f{static_cast<float>(config.setup.cellWidth), static_cast<float>(config.setup.cellHeight)});
        });
}

void SimulationWidget::render(const FrameDTO& frame, const cell::DiscTypeResolver& discTypeResolver,
                              const std::map<std::string, sf::Color>& colorMap)
{
    const int FPS = 120;
    if (frame.elapsedSimulationTimeUs > 0 && clock_.getElapsedTime() < sf::seconds(1.f / FPS))
        return;

    clock_.restart();

    sf::RenderWindow::clear(sf::Color::Black);

    for (const auto& disc : frame.discs_)
    {
        const auto& discType = discTypeResolver(disc.getDiscTypeID());

        sf::CircleShape circleShape;
        circleShape.setPointCount(50);
        circleShape.setPosition(static_cast<sf::Vector2f>(disc.getPosition()));
        circleShape.setRadius(discType.getRadius());
        circleShape.setFillColor(colorMap.at(discType.getName()));
        circleShape.setOrigin(
            sf::Vector2f{static_cast<float>(discType.getRadius()), static_cast<float>(discType.getRadius())});

        sf::RenderWindow::draw(circleShape);
    }

    boundingRect_.setOutlineThickness(static_cast<float>(QSFMLWidget::getCurrentZoom()));

    sf::RenderWindow::draw(boundingRect_);
    sf::RenderWindow::display();
}
