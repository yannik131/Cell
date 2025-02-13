#ifndef SIMULATIONWIDGET_HPP
#define SIMULATIONWIDGET_HPP

#include "QSFMLWidget.hpp"
#include "FrameDTO.hpp"
#include "Disc.hpp"

#include <SFML/System/Clock.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/CircleShape.hpp>

class SimulationWidget : public QSFMLWidget
{
    Q_OBJECT
public:
    SimulationWidget(QWidget* parent);

public slots:
    void render(const FrameDTO& frameDTO);
    void initialize(const std::vector<Disc>& discs);

private:
    std::vector<sf::CircleShape> circles_;
};

#endif /* SIMULATIONWIDGET_HPP */
