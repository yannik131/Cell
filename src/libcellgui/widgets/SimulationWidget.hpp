#ifndef SIMULATIONWIDGET_HPP
#define SIMULATIONWIDGET_HPP

#include "Disc.hpp"
#include "FrameDTO.hpp"
#include "QSFMLWidget.hpp"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Clock.hpp>

class SimulationWidget : public QSFMLWidget
{
    Q_OBJECT
public:
    SimulationWidget(QWidget* parent);

public slots:
    void render(const FrameDTO& frameDTO);

private:
    sf::CircleShape circleShapeFromDisc(const Disc& disc);

private:
    std::vector<sf::CircleShape> circles_;
    sf::Clock clock_;
};

#endif /* SIMULATIONWIDGET_HPP */
