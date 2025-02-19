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

void SimulationWidget::removeAndChangeDiscs(const UpdateDTO& updateDTO)
{
    // Order matters here, changed discs refer to the new indices
    // TODO Maybe copy with move(circles[i]) and skip destroyed elements?
    for (auto iter = updateDTO.destroyedDiscsIndexes_.rbegin(); iter != updateDTO.destroyedDiscsIndexes_.rend(); ++iter)
        circles_.erase(circles_.begin() + *iter);

    for (const auto& [index, discType] : updateDTO.changedDiscsIndices_)
        circles_[index] = circleShapeFromDisc(Disc(discType));
}

sf::CircleShape SimulationWidget::circleShapeFromDisc(const Disc& disc)
{
    sf::CircleShape shape(disc.type_.radius_);
    shape.setFillColor(disc.type_.color_);
    shape.setOrigin(sf::Vector2f(disc.type_.radius_, disc.type_.radius_));
    shape.setPosition(disc.position_);

    return shape;
}
