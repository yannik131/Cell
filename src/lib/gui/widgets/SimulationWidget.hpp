#ifndef SIMULATIONWIDGET_HPP
#define SIMULATIONWIDGET_HPP

#include "Disc.hpp"
#include "FrameDTO.hpp"
#include "QSFMLWidget.hpp"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Clock.hpp>

/**
 * @brief Widget displaying the simulation state by drawing a bunch of circles in different colors each frame based on
 * what is currently in the simulation
 */
class SimulationWidget : public QSFMLWidget
{
    Q_OBJECT
public:
    SimulationWidget(QWidget* parent);

public slots:
    /**
     * @brief Clears the render window and displays circles based on the given `FrameDTO`
     */
    void render(const FrameDTO& frameDTO);

private:
    /**
     * @brief Creates a `sf::CircleShape` from a disc, reflecting its properties like size and color
     */
    sf::CircleShape circleShapeFromDisc(const cell::Disc& disc);

private:
    std::vector<sf::CircleShape> circles_;
    sf::Clock clock_;
    sf::RectangleShape boundingRect_;
};

#endif /* SIMULATIONWIDGET_HPP */
