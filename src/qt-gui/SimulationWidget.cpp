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
    // Order matters here, changed discs refer to the old indices, so we can't remove anything yet
    if (!frameDTO.changedDiscsIndices_.empty())
        changeDiscs(frameDTO.changedDiscsIndices_);

    if (!frameDTO.destroyedDiscsIndexes_.empty())
        removeDestroyedDiscs(frameDTO.destroyedDiscsIndexes_);

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

void SimulationWidget::removeDestroyedDiscs(const std::vector<int>& indices)
{
    // TODO use unordered_set for discs, give discs id as static int instanceCount, use id as hash value
    // TODO or maybe set and use the order for drawing?
    for (const auto& index : indices)
        circles_.erase(circles_.begin() + index);
}

void SimulationWidget::changeDiscs(const std::vector<std::pair<int, DiscType>>& changedDiscsIndices)
{
    // Position will be left unitialized
    for (const auto& [index, discType] : changedDiscsIndices)
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
