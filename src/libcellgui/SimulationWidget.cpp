#include "SimulationWidget.hpp"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <glog/logging.h>

#include <map>

SimulationWidget::SimulationWidget(QWidget* parent)
    : QSFMLWidget(parent)
{
}

void SimulationWidget::render(const FrameDTO& frameDTO)
{
    VLOG(1) << "Rendering, received positions of " << frameDTO.discs_.size() << " discs.";
    VLOG(1) << "Rendering, we have " << circles_.size() << " circle shapes.";

    sf::RenderWindow::clear(sf::Color::Black);

    for (int i = 0; i < circles_.size(); ++i)
    {
        auto& circleShape = circles_[i];
        const auto& guiDisc = frameDTO.discs_[i];
        circleShape.setPosition(guiDisc.position_);

        sf::RenderWindow::draw(circleShape);
    }

    sf::RenderWindow::display();
}

void SimulationWidget::initialize(const std::vector<Disc>& discs)
{
    if (circles_.empty())
        circles_.reserve(discs.size());
    else
        circles_.clear();

    for (const auto& disc : discs)
        circles_.push_back(circleShapeFromDisc(disc));
}

void SimulationWidget::update(const UpdateDTO& updateDTO)
{
    // We need to do this in the same order it happens in the simulation:
    // 1. Add new discs 2. Remove destroyed discs 3. Update changed discs
    for (const auto& disc : updateDTO.newDiscs_)
        circles_.push_back(circleShapeFromDisc(disc));

    // TODO Maybe copy with move(circles[i]) and skip destroyed elements?
    for (auto iter = updateDTO.destroyedDiscsIndexes_.rbegin(); iter != updateDTO.destroyedDiscsIndexes_.rend(); ++iter)
        circles_.erase(circles_.begin() + *iter);

    for (const auto& [index, discType] : updateDTO.changedDiscsIndices_)
        circles_[index] = circleShapeFromDisc(Disc(discType));
}

sf::CircleShape SimulationWidget::circleShapeFromDisc(const Disc& disc)
{
    sf::CircleShape shape(disc.getType().radius_);
    shape.setFillColor(disc.getType().color_);
    shape.setOrigin(sf::Vector2f(disc.getType().radius_, disc.getType().radius_));
    shape.setPosition(disc.getPosition());

    return shape;
}
